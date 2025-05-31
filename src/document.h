#ifndef DOCUMENT_H
#define DOCUMENT_H

// Estructura para representar un documento en la lista enlazada
typedef struct Document {
  char *filename;        // Ruta completa del archivo
  struct Document *next; // Siguiente documento en la lista
} Document;

// Devolver el nombre del archivo sin la ruta (extraer la parte final de
// filepath)
const char *extract_filename(const char *filepath);

// Crear un nuevo Document a partir de la ruta
Document *create_document(const char *filepath);

// Función recursiva para buscar archivos .txt en la carpeta y sus subcarpetas
// Esta versión acumula todos los Document * en un arreglo dinámico para
// ordenarlos
void search_in_subfolders(const char *folder, Document ***doc_array, int *count,
                          int *capacity);

// Cargar todos los documentos a partir de la carpeta
Document *load_documents(const char *folder);

// Muestra por pantalla el contenido de un archivo
void view_document_content(const char *filename);

// Imprimir la lista de documentos (con índices)
void print_documents(Document *head);

// Liberar la memoria de la lista de Document
void free_documents(Document *head);

#endif // DOCUMENT_H