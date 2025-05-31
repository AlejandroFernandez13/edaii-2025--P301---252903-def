#include "document.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Extraer el nombre del archivo de una ruta completa
const char *extract_filename(const char *filepath) {
  const char *last_slash = strrchr(filepath, '/');
  return (last_slash) ? last_slash + 1 : filepath;
}

// Crear un documento asignando memoria y copiando la ruta
Document *create_document(const char *filepath) {
  Document *new_doc = malloc(sizeof(Document));
  if (!new_doc) {
    perror("Error allocating memory for document");
    return NULL;
  }
  new_doc->filename = strdup(filepath);
  new_doc->next = NULL;
  return new_doc;
}

// Función recursiva para buscar archivos .txt en folder y subcarpetas.
// Se acumulan los punteros a Document en un arreglo dinámico (doc_array).
void search_in_subfolders(const char *folder, Document ***doc_array, int *count,
                          int *capacity) {
  DIR *dir = opendir(folder);
  if (!dir) {
    perror("Error opening directory");
    return;
  }
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    // Omitir "." y ".."
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    char path[512];
    snprintf(path, sizeof(path), "%s/%s", folder, entry->d_name);
    struct stat path_stat;
    if (stat(path, &path_stat) == -1)
      continue;

    if (S_ISDIR(path_stat.st_mode)) {
      // Recorrer la subcarpeta
      search_in_subfolders(path, doc_array, count, capacity);
    } else if (S_ISREG(path_stat.st_mode)) {
      // Si es archivo y su nombre contiene ".txt"
      if (strstr(entry->d_name, ".txt") != NULL) {
        if (*count >= *capacity) {
          *capacity *= 2;
          *doc_array = realloc(*doc_array, (*capacity) * sizeof(Document *));
          if (!*doc_array) {
            perror("Error reallocating doc_array");
            closedir(dir);
            return;
          }
        }
        (*doc_array)[*count] = create_document(path);
        (*count)++;
      }
    }
  }
  closedir(dir);
}

// Cargar documentos desde folder y devolver una lista enlazada ordenada
// alfabéticamente
Document *load_documents(const char *folder) {
  int capacity = 100;
  int count = 0;
  Document **doc_array = malloc(capacity * sizeof(Document *));
  if (!doc_array) {
    perror("Error allocating doc_array");
    return NULL;
  }
  search_in_subfolders(folder, &doc_array, &count, &capacity);

  if (count == 0) {
    free(doc_array);
    return NULL;
  }

  // Ordenar el arreglo usando qsort
  int compare_documents(const void *a, const void *b) {
    Document *docA = *(Document **)a;
    Document *docB = *(Document **)b;
    return strcmp(docA->filename, docB->filename);
  }
  qsort(doc_array, count, sizeof(Document *), compare_documents);

  // Reconstruir la lista enlazada a partir del arreglo ordenado
  for (int i = 0; i < count - 1; i++) {
    doc_array[i]->next = doc_array[i + 1];
  }
  doc_array[count - 1]->next = NULL;
  Document *head = doc_array[0];
  free(doc_array);
  return head;
}

// Abrir y mostrar el contenido de un archivo
void view_document_content(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    printf("Error opening file: %s\n", filename);
    return;
  }
  printf("\n--- Content of %s ---\n", filename);
  char line[512];
  while (fgets(line, sizeof(line), file)) {
    printf("%s", line);
  }
  printf("\n----------------------\n");
  fclose(file);
}

// Imprimir la lista de documentos con índice
void print_documents(Document *head) {
  int index = 0;
  Document *current = head;
  printf("\nAvailable Documents:\n");
  while (current) {
    printf("[%d] %s\n", index, current->filename);
    current = current->next;
    index++;
  }
}

// Liberar la memoria reservada para cada documento en la lista
void free_documents(Document *head) {
  Document *current = head;
  while (current) {
    Document *next = current->next;
    free(current->filename);
    free(current);
    current = next;
  }
}