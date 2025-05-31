#ifndef QUERY_H
#define QUERY_H

#include "document.h" // Para poder relacionar queries con documentos

// Estructura simple para representar una consulta
typedef struct Query {
  char *text;
} Query;

// Crear una nueva consulta a partir de un string
Query *create_query(const char *text);

// Buscar documentos que contengan el string de la query en su filename.
// Guardar hasta max_results en el arreglo results y devolver el número de
// documentos encontrados.
int query_documents(Document *docs, Query *query, Document **results,
                    int max_results);

// Liberar la memoria asociada a una consulta.
void free_queries(Query *query);

#endif // QUERY_H