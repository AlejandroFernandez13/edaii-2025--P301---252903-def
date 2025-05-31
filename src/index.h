#ifndef INDEX_H
#define INDEX_H

#include "document.h"
#include "document_graph.h" 

#define HASHMAP_SIZE 10000  

// Cada entrada del índice asocia una palabra con un arreglo de índices de documentos
typedef struct IndexEntry {
    char *word;         // La palabra indexada
    int *documentIds;   // Arreglo de índices de documentos en los que aparece la palabra
    int count;          // Número de documentos encontrados para esta palabra
    struct IndexEntry *next;  // Para manejar colisiones (lista enlazada)
} IndexEntry;

// El HashMap es un arreglo de punteros a IndexEntry
typedef struct HashMap {
    IndexEntry *entries[HASHMAP_SIZE];
} HashMap;

// Crear e inicializar un nuevo HashMap
HashMap *create_hashmap();

// Recorrer la lista enlazada de documentos y agregar todas las palabras (de los filenames) al índice inverso
void index_documents(HashMap *map, DocumentGraph *graph, Document *docs);

// Buscar en el índice una palabra exacta (en minúsculas) y devolver el arreglo de índices de documentos y el número de resultados encontrados
int *search_index(HashMap *map, const char *query, int *num_results);

// Liberar la memoria del HashMap y todas sus entradas
void free_hashmap(HashMap *map);

int compare_documents(const void *a, const void *b);

#endif  // INDEX_H