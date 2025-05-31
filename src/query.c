#include "query.h"
#include "document_graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  Document *doc;
  float score;
} SearchResult;

extern DocumentGraph *global_graph;

float alpha = 1.0; // Ajustar el valor del indegree

int get_doc_id(Document *doc) {

  int id = 0;
  sscanf(doc->filename, "%d", &id);
  return id;
}

int compare_results(const void *a, const void *b) {
  const SearchResult *ra = (const SearchResult *)a;
  const SearchResult *rb = (const SearchResult *)b;

  int id_a = get_doc_id(ra->doc);
  int id_b = get_doc_id(rb->doc);

  float score_a = ra->score + alpha * graphGetIndegree(global_graph, id_a);
  float score_b = rb->score + alpha * graphGetIndegree(global_graph, id_b);

  if (score_a > score_b)
    return -1;
  if (score_a < score_b)
    return 1;
  return 0;
}

Query *create_query(const char *text) {
  Query *q = malloc(sizeof(Query));
  if (!q) {
    perror("Error allocating query");
    return NULL;
  }
  q->text = strdup(text);
  return q;
}

int query_documents(Document *docs, Query *query, Document **results,
                    int max_results) {
  int count = 0;
  Document *current = docs;
  SearchResult temp_results[max_results];

  printf("Buscando '%s' dentro de los documentos...\n", query->text);

  while (current && count < max_results) {
    FILE *file = fopen(current->filename, "r");
    if (!file) {
      perror("Error abriendo documento");
      current = current->next;
      continue;
    }

    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
      if (strstr(line, query->text) != NULL) {
        found = 1;
        break;
      }
    }
    fclose(file);

    if (found) {
      temp_results[count].doc = current;
      temp_results[count].score = 1.0;
      count++;
    }
    current = current->next;
  }

  // Ordenar los resultados por score + indegree
  qsort(temp_results, count, sizeof(SearchResult), compare_results);

  // Llenar el arreglo de resultados finales
  for (int i = 0; i < count; i++) {
    results[i] = temp_results[i].doc;
  }

  return count;
}

void free_queries(Query *query) {
  if (query) {
    free(query->text);
    free(query);
  }
}