# Report: Building a search engine like Google

1) DIAGRAMA DE COMPONENTES

![image](https://github.com/user-attachments/assets/5574d8dc-10ee-4809-8f0b-27b5ae532b70)

Explicación:
El diagrama representa el Sistema de Búsqueda, dividiéndolo en tres módulos principales:

- Gestión de Documentos:

Document: Representa los archivos individuales a analizar.

DocumentGraph / DocumentNode: Modela los documentos como un grafo, conectando archivos mediante enlaces internos.

- Módulo de Consulta:

Query: Maneja las búsquedas en el sistema, proporcionando términos clave para encontrar documentos relevantes.

- Índice Inverso:

HashMap / IndexEntry: Estructura que almacena asociaciones entre palabras clave y los documentos en los que aparecen.



Interacción:

Los documentos se transforman en una estructura de grafo para analizar relaciones y relevancia.

Las consultas utilizan el índice inverso para localizar rápidamente documentos relevantes.

El índice se actualiza dinámicamente con la información del grafo y las consultas.


Todas las estructuras son volátiles, se almacenan en la RAM y desaparecen al cerrar el programa. Los documentos en que se basa la estructura Document sí son persistentes.

2) TABLA

![image](https://github.com/user-attachments/assets/89944a8d-32fb-4f83-9f6e-39315f01dd76)


3) PLOTS

a) Tiempo de búsqueda con/sin índice inverso: 
Con índice inverso, el tiempo de búsqueda disminuye, y la diferencia se aprecia más cuanto mayor es la cantidad de archivos a analizar.

b) Tiempo de inicialización para diferentes configuraciones de ranuras en el HashMap: 
Existe un número óptimo de ranuras en el que el tiempo de inicialización es mínimo; con menos ranura se generan más colisiones, y con un exceso se incrementa la sobrecarga en la asignación de memoria.

c) Tiempo de búsqueda para diferentes configuraciones de ranuras en el HashMap:
En una configuración óptima, el tiempo de búsqueda es el menor, mientras que configuraciones fuera de este rango provocan una mayor latencia debido a colisiones excesivas o a un uso ineficiente de la memoria.

4) MEJORAS

Índice Inverso:

Mejora: Explorar alternativas de estructura de datos como árboles balanceados para optimizar búsquedas complejas.

Explicación: Usar árboles balanceados para el índice inverso consumiría más memoria en comparación con una estructura hash simple. Esto se debe a que los árboles almacenan enlaces adicionales entre nodos para mantener su balance y pueden requerir operaciones de reestructuración al insertar o eliminar datos. Su complejidad sería o(log n), y aunque sería más eficiente al buscar ordenación dinámica y estabilidad, también tomaría más tiempo que la alternativa actual.

> [!NOTE]  
> Complete the report in this file before delivering the project.
> The lab guide has a list of the questions you must answer.

