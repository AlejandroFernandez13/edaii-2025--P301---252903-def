#include "index.h"
#include "document_graph.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Acceder al grafo global
extern DocumentGraph *global_graph;
extern Document *document_list; // Asegurar que está declarada globalmente

// Función simple de hash, basada en multiplicadores
unsigned int hash_function(const char *str) {
  unsigned int hash = 0;
  for (; *str; str++) {
    hash = (hash * 31) + tolower((unsigned char)*str);
  }
  return hash % HASHMAP_SIZE;
}

HashMap *create_hashmap() {
  HashMap *map = malloc(sizeof(HashMap));
  if (!map) {
    perror("Error al asignar memoria para el hashmap");
    return NULL;
  }
  for (int i = 0; i < HASHMAP_SIZE; i++) {
    map->entries[i] = NULL;
  }
  return map;
}

// Indexar documentos y registrar enlaces en el grafo dirigido
void index_documents(HashMap *map, DocumentGraph *graph, Document *docs) {
  int doc_id = 0; // Asegurar que los documentos sean indexados secuencialmente

  for (Document *d = docs; d != NULL; d = d->next, doc_id++) {
    FILE *file = fopen(d->filename, "r");
    if (!file)
      continue;

    char word[64];
    char indexed_words[500][64]; // Registro de palabras ya indexadas en este
                                 // documento
    int indexed_count = 0;

    while (fscanf(file, "%63s", word) == 1) {
      // Detectar enlaces en el texto (formato: "(ID)")
      if (word[0] == '(') {
        int linked_doc = atoi(
            word + 1); // Obtener el ID del documento al que apunta el enlace
        if (linked_doc >= 0) {
          add_link(graph, doc_id,
                   linked_doc); // Agregar relación en el grafo dirigido
        }
      }

      // Verificar si esta palabra ya se indexó en este documento
      int already_indexed = 0;
      for (int i = 0; i < indexed_count; i++) {
        if (strcmp(indexed_words[i], word) == 0) {
          already_indexed = 1;
          break;
        }
      }
      if (already_indexed)
        continue;

      unsigned int hash = hash_function(word);
      IndexEntry *entry = map->entries[hash];

      while (entry) {
        if (strcmp(entry->word, word) == 0) {
          entry->documentIds =
              realloc(entry->documentIds, (entry->count + 1) * sizeof(int));

          // Asegurar que los documentos se agreguen en orden numérico
          // ascendente
          int pos = entry->count;
          while (pos > 0 && entry->documentIds[pos - 1] > doc_id) {
            entry->documentIds[pos] = entry->documentIds[pos - 1];
            pos--;
          }
          entry->documentIds[pos] = doc_id;

          entry->count++;
          break;
        }
        entry = entry->next;
      }

      if (!entry) {
        entry = malloc(sizeof(IndexEntry));
        entry->word = strdup(word);
        entry->documentIds = malloc(sizeof(int));
        entry->documentIds[0] = doc_id;
        entry->count = 1;
        entry->next = map->entries[hash];
        map->entries[hash] = entry;
      }

      // Agregar la palabra a la lista de palabras indexadas
      strncpy(indexed_words[indexed_count++], word, sizeof(word));
    }
    fclose(file);
  }
}

// Comparación de documentos por relevancia (indegree en el grafo)
int compare_by_relevance(const void *a, const void *b) {
  int docA = *(int *)a;
  int docB = *(int *)b;
  float relevanceA = graphGetIndegree(global_graph, docA);
  float relevanceB = graphGetIndegree(global_graph, docB);

  return relevanceB - relevanceA; // Orden descendente
}

// Búsqueda en el índice inverso y ordenar así por relevancia
int *search_index(HashMap *map, const char *query, int *num_results) {
  char q_copy[256];
  strncpy(q_copy, query, sizeof(q_copy));
  q_copy[sizeof(q_copy) - 1] = '\0';
  for (char *p = q_copy; *p; p++) {
    *p = tolower(*p);
  }

  unsigned int hash = hash_function(q_copy);
  IndexEntry *entry = map->entries[hash];

  while (entry) {
    if (strcmp(entry->word, q_copy) == 0) {
      *num_results = entry->count;

      // Eliminamos IDs duplicados
      int *unique_doc_ids = malloc(entry->count * sizeof(int));
      int unique_count = 0;

      for (int i = 0; i < entry->count; i++) {
        int is_duplicate = 0;
        for (int j = 0; j < unique_count; j++) {
          if (unique_doc_ids[j] == entry->documentIds[i]) {
            is_duplicate = 1;
            break;
          }
        }
        if (!is_duplicate) {
          unique_doc_ids[unique_count++] = entry->documentIds[i];
        }
      }

      *num_results = unique_count;

      // Ordenamos los resultados por relevancia antes de devolverlos
      qsort(unique_doc_ids, unique_count, sizeof(int), compare_by_relevance);

      return unique_doc_ids;
    }
    entry = entry->next;
  }

  *num_results = 0;
  return NULL;
}

// Comparación de documentos por nombre de archivo
int compare_documents(const void *a, const void *b) {
  int docA = *(int *)a;
  int docB = *(int *)b;
  return docA - docB; // Orden numérico ascendente

  Document *currentA = document_list;
  Document *currentB = document_list;
  int indexA = 0, indexB = 0;

  while (currentA && indexA < docA) {
    currentA = currentA->next;
    indexA++;
  }
  while (currentB && indexB < docB) {
    currentB = currentB->next;
    indexB++;
  }

  return strcmp(currentA->filename, currentB->filename);
}

// Liberar la memoria de todas las entradas del hash y del propio HashMap
void free_hashmap(HashMap *map) {
  if (!map)
    return;
  for (int i = 0; i < HASHMAP_SIZE; i++) {
    IndexEntry *entry = map->entries[i];
    while (entry) {
      IndexEntry *next = entry->next;
      free(entry->word);
      free(entry->documentIds);
      free(entry);
      entry = next;
    }
  }
  free(map);
}