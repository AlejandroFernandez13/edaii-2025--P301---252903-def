#include "document.h"
#include "document_graph.h"
#include "index.h"
#include "query.h"
#include "sample_lib.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variable global para la lista de documentos
Document *document_list = NULL;

// Variable global para el grafo dirigido de documentos
DocumentGraph *global_graph = NULL;

void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

int main() {

  // Inicializar el grafo dirigido
  global_graph = create_graph();
  if (!global_graph) {
    printf("Error al inicializar el grafo.\n");
    return 1;
  }

  // Cargar documentos desde la carpeta "datasets/"
  document_list = load_documents("datasets/");
  if (!document_list) {
    printf("No se encontraron documentos en datasets.\n");
    return 1;
  }

  // Crear el índice inverso (hashmap) para la búsqueda eficiente (opción 3)
  HashMap *rev_index = create_hashmap();
  if (rev_index == NULL) {
    printf("Error al crear el índice inverso.\n");
  } else {
    index_documents(rev_index, global_graph, document_list);
  }

  int option = 0;
  while (1) {
    // Menú principal
    printf("\n================== MENÚ ==================\n");
    printf("Seleccione una opción:\n");
    printf("1. Elegir documento por índice (lista completa)\n");
    printf("2. Elegir por query (búsqueda básica)\n");
    printf("3. Elegir con búsqueda eficiente (índice inverso)\n");
    printf("4. Salir\n");
    printf("==========================================\n");
    printf("Ingrese su opción: ");

    if (scanf("%d", &option) != 1) {
      // Limpiar entrada en caso de error
      while (getchar() != '\n')
        ;
      printf("Entrada inválida\n");
      continue;
    }
    while (getchar() != '\n')
      ; // Limpiar buffer

    if (option == 4) {
      printf("Saliendo del programa...\n");
      break;
    }

    if (option == 1) {
      // Opción 1: Elegir documento por índice (lista completa)
      print_documents(document_list);
      int idx;
      printf("Ingrese el índice del documento a visualizar: ");
      if (scanf("%d", &idx) != 1) {
        while (getchar() != '\n')
          ;
        printf("Entrada inválida.\n");
        continue;
      }
      while (getchar() != '\n')
        ;

      // Recorrer la lista para buscar el documento por índice
      Document *d = document_list;
      int current = 0;
      while (d && current < idx) {
        d = d->next;
        current++;
      }
      if (d) {
        view_document_content(d->filename);
      } else {
        printf("Índice no válido.\n");
      }
    } else if (option == 2) {
      // Opción 2: Elegir por query (búsqueda básica)
      char query_text[256];
      printf("Ingrese query: ");
      if (fgets(query_text, sizeof(query_text), stdin) == NULL) {
        continue;
      }
      size_t len = strlen(query_text);
      if (len > 0 && query_text[len - 1] == '\n')
        query_text[len - 1] = '\0';

      // Crear query
      Query *query = create_query(query_text);
      Document *results[5];
      int found_count = query_documents(document_list, query, results, 5);
      if (found_count == 0) {
        printf("No se encontraron resultados para '%s'.\n", query_text);
      } else {
        printf("Resultados encontrados (máximo 5):\n");
        for (int i = 0; i < found_count && i < 5; i++) {
          // Usamos extract_filename para mostrar solo el nombre (sin la ruta
          // completa)
          const char *fname = extract_filename(results[i]->filename);
          printf("[%d] %s\n", i, fname);
        }
        int sel;
        printf("Seleccione el índice del resultado a visualizar: ");
        if (scanf("%d", &sel) != 1) {
          while (getchar() != '\n')
            ;
          printf("Entrada inválida.\n");
        } else {
          while (getchar() != '\n')
            ;
          if (sel >= 0 && sel < found_count) {
            view_document_content(results[sel]->filename);
          } else {
            printf("Índice seleccionado no es válido.\n");
          }
        }
      }
      free_queries(query);
    } else if (option == 3) {
      // Opción 3: Elegir con búsqueda eficiente (usando índice inverso)
      if (rev_index == NULL) {
        printf("El índice inverso no está disponible.\n");
      } else {
        char query_text[256];
        printf("Ingrese query para búsqueda eficiente: ");
        if (fgets(query_text, sizeof(query_text), stdin) == NULL)
          continue;
        size_t len = strlen(query_text);
        if (len > 0 && query_text[len - 1] == '\n')
          query_text[len - 1] = '\0';

        int num_results;
        int *doc_ids = search_index(rev_index, query_text, &num_results);

        // Verificar si todos los indegree son 0
        bool todos_cero = true;
        for (int i = 0; i < num_results; i++) {
          if (graphGetIndegree(global_graph, doc_ids[i]) > 0) {
            todos_cero = false;
            break;
          }
        }

        // Si todos los indegree son 0, ordenar doc_ids de menor a mayor
        if (todos_cero) {
          for (int i = 0; i < num_results - 1; i++) {
            for (int j = i + 1; j < num_results; j++) {
              if (doc_ids[i] > doc_ids[j]) {
                int tmp = doc_ids[i];
                doc_ids[i] = doc_ids[j];
                doc_ids[j] = tmp;
              }
            }
          }
        }

        if (num_results == 0) {
          printf("No se encontraron documentos con '%s' usando búsqueda "
                 "eficiente.\n",
                 query_text);
        } else {
          printf("Resultados encontrados (ordenados por relevancia):\n");
          float alpha = 1.0; // Debe coincidir con el valor usado en query.c
          float score_textual = 1.0; // Score base para coincidencia de búsqueda
          for (int i = 0; i < num_results && i < 5; i++) {
            Document *d = document_list;
            int j = 0;
            while (d && j < doc_ids[i]) {
              d = d->next;
              j++;
            }
            if (d) {
              float relevancia =
                  score_textual +
                  alpha * graphGetIndegree(global_graph, doc_ids[i]);
              printf("[%d] %s - Relevancia: %.1f\n", i,
                     extract_filename(d->filename), relevancia);
            }
          }
          // Permitir seleccionar un documento para visualizar
          int sel;
          printf("Seleccione el índice del resultado a visualizar: ");
          if (scanf("%d", &sel) != 1) {
            while (getchar() != '\n')
              ;
            printf("Entrada inválida.\n");
          } else {
            while (getchar() != '\n')
              ;
            if (sel >= 0 && sel < num_results && sel < 5) {
              // Buscar el Document* correspondiente
              Document *d = document_list;
              int j = 0;
              while (d && j < doc_ids[sel]) {
                d = d->next;
                j++;
              }
              if (d) {
                view_document_content(d->filename);
              } else {
                printf("No se pudo encontrar el documento seleccionado.\n");
              }
            } else {
              printf("Índice seleccionado no es válido.\n");
            }
          }
        }
        free(doc_ids);
      }
    } else {
      printf("Opción no válida. Por favor, ingrese 1, 2, 3 o 4 para salir.\n");
    }
  }

  // Liberar recursos antes de salir
  free_documents(document_list);
  free_hashmap(rev_index);
  free(global_graph);

  printf("Memoria liberada correctamente. ¡Hasta pronto!\n");
  return 0;
}
