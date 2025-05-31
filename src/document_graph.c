#include "document_graph.h"
#include <stdio.h>

// Crear un grafo cuyos nodos se inicializan en NULL
DocumentGraph *create_graph() {
  DocumentGraph *graph = malloc(sizeof(DocumentGraph));
  if (!graph) {
    perror("Error al asignar memoria para el grafo");
    return NULL;
  }
  for (int i = 0; i < MAX_DOCUMENTS; i++) {
    graph->nodes[i] = NULL;
  }
  return graph;
}

// Agregar un enlace dirigido desde 'fromDoc' a 'toDoc'
// (El parámetro 'fromDoc' nos permite registrar de dónde viene el enlace,
// aunque para el cálculo de relevancia solo necesitamos incrementar el indegree
// del documento destino)
void add_link(DocumentGraph *graph, int fromDoc, int toDoc) {
  if (toDoc < 0 || toDoc >= MAX_DOCUMENTS || fromDoc < 0 ||
      fromDoc >= MAX_DOCUMENTS)
    return; // Si el ID de origen o destino está fuera de rango, se ignora

  // Si aún no existe un nodo para 'toDoc', lo creamos
  if (graph->nodes[toDoc] == NULL) {
    graph->nodes[toDoc] = malloc(sizeof(DocumentNode));
    if (!graph->nodes[toDoc]) {
      perror("Error al asignar memoria para el nodo destino");
      return;
    }
    graph->nodes[toDoc]->documentId = toDoc;
    graph->nodes[toDoc]->indegree = 0;
    graph->nodes[toDoc]->next = NULL;
  }

  // Si aún no existe un nodo para 'fromDoc', lo creamos (para rastrear enlaces
  // salientes)
  if (graph->nodes[fromDoc] == NULL) {
    graph->nodes[fromDoc] = malloc(sizeof(DocumentNode));
    if (!graph->nodes[fromDoc]) {
      perror("Error al asignar memoria para el nodo origen");
      return;
    }
    graph->nodes[fromDoc]->documentId = fromDoc;
    graph->nodes[fromDoc]->indegree = 0;
    graph->nodes[fromDoc]->next = NULL;
  }

  // Incrementar la relevancia del documento destino.
  graph->nodes[toDoc]->indegree++;

  // Registrar el enlace desde 'fromDoc' a 'toDoc' en la lista de conexiones
  DocumentNode *newNode = malloc(sizeof(DocumentNode));
  if (!newNode) {
    perror("Error al asignar memoria para un enlace en el grafo");
    return;
  }
  newNode->documentId = toDoc;
  newNode->indegree = 0; // No se usa aquí, solo en nodos principales
  newNode->next = graph->nodes[fromDoc]->next;
  graph->nodes[fromDoc]->next = newNode;
}

// Devolver el número de enlaces entrantes para un documento (su "relevancia")
float graphGetIndegree(DocumentGraph *graph, int documentId) {
  if (!graph || documentId < 0 || documentId >= MAX_DOCUMENTS ||
      graph->nodes[documentId] == NULL)
    return 0.0;
  return (float)graph->nodes[documentId]->indegree;
}