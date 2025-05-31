#ifndef DOCUMENT_GRAPH_H
#define DOCUMENT_GRAPH_H

#include <stdlib.h>

#define MAX_DOCUMENTS 5000  // Número máximo de documentos a tener en cuenta

typedef struct DocumentNode {
    int documentId;
    int indegree;                // Cantidad de enlaces entrantes
    struct DocumentNode *next;   
} DocumentNode;

typedef struct DocumentGraph {
    DocumentNode *nodes[MAX_DOCUMENTS];
} DocumentGraph;

// Funciones del grafo
DocumentGraph *create_graph();
void add_link(DocumentGraph *graph, int fromDoc, int toDoc);
float graphGetIndegree(DocumentGraph *graph, int documentId);

#endif // DOCUMENT_GRAPH_H