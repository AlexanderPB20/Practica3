#include "vertex.h"
#include "graph.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_VTX 4096
#define TAG 64

struct _Graph {
    Vertex *vertices[MAX_VTX];
    Bool connections[MAX_VTX][MAX_VTX];
    int num_vertices;
    int num_edges;
};

Graph *graph_init() {
    Graph *g = malloc(sizeof(Graph));
    int i, j;

    if (g == NULL) {
        return NULL;
    }

    g->num_edges = 0;
    g->num_vertices = 0;

    for (i = 0; i < MAX_VTX; i++) {
        for (j = 0; j < MAX_VTX; j++) {
            g->connections[i][j] = FALSE;
        }
    }

    return g;
}

void graph_free(Graph *g) {
    int i;

    if (g != NULL) {
        for (i = 0; i < g->num_vertices; i++) {
            if (g->vertices[i] != NULL) {
                vertex_free(g->vertices[i]);
            }
        }
        free(g);
    }
}

Status graph_newVertex(Graph *g, char *desc) {
    int i;
    long id;
    Vertex *v = NULL;

    if (g == NULL || desc == NULL) {
        return ERROR;
    }

    v = vertex_initFromString(desc);
    if (v == NULL) {
        return ERROR;
    }

    id = vertex_getId(v);

    for (i = 0; i < g->num_vertices; i++) {
        if (vertex_getId(g->vertices[i]) == id) {
            vertex_free(v);
            return OK;
        }
    }

    g->vertices[g->num_vertices] = v;
    g->num_vertices++;

    return OK;
}

Status graph_newEdge(Graph *g, long orig, long dest) {
    int i, j;

    if (g == NULL || orig == dest) {
        return ERROR;
    }

    for (i = 0; i < g->num_vertices; i++) {
        if (vertex_getId(g->vertices[i]) == orig) {
            for (j = 0; j < g->num_vertices; j++) {
                if (vertex_getId(g->vertices[j]) == dest) {
                    g->connections[i][j] = TRUE;
                    g->num_edges++;
                    return OK;
                }
            }
        }
    }

    return ERROR;
}

Bool graph_contains(const Graph *g, long id) {
    int i;

    if (g == NULL) {
        return FALSE;
    }

    for (i = 0; i < g->num_vertices; i++) {
        if (vertex_getId(g->vertices[i]) == id) {
            return TRUE;
        }
    }

    return FALSE;
}

int graph_getNumberOfVertices(const Graph *g) {
    if (g == NULL) {
        return -1;
    }
    return g->num_vertices;
}

int graph_getNumberOfEdges(const Graph *g) {
    if (g == NULL) {
        return -1;
    }
    return g->num_edges;
}

Bool graph_connectionExists(const Graph *g, long orig, long dest) {
    int i, j;

    if (g == NULL || orig == dest) {
        return FALSE;
    }

    for (i = 0; i < g->num_vertices; i++) {
        if (vertex_getId(g->vertices[i]) == orig) {
            for (j = 0; j < g->num_vertices; j++) {
                if (vertex_getId(g->vertices[j]) == dest) {
                    if (g->connections[i][j] == TRUE) {
                        return TRUE;
                    }
                    return FALSE;
                }
            }
        }
    }

    return FALSE;
}

int graph_getNumberOfConnectionsFromId(const Graph *g, long id) {
    int i, j, n = 0;

    if (g == NULL) {
        return -1;
    }

    for (i = 0; i < g->num_vertices; i++) {
        if (vertex_getId(g->vertices[i]) == id) {
            for (j = 0; j < g->num_vertices; j++) {
                if (g->connections[i][j] == TRUE) {
                    n++;
                }
            }
            break;
        }
    }

    return n;
}

long *graph_getConnectionsFromId(const Graph *g, long id) {
    long *connections = NULL;
    int n, i, j, x;

    if (g == NULL) {
        return NULL;
    }

    n = graph_getNumberOfConnectionsFromId(g, id);
    if (n == -1) {
        return NULL;
    }

    connections = (long *)malloc(n * sizeof(long));
    if (connections == NULL) {
        return NULL;
    }

    x = 0;
    for (i = 0; i < g->num_vertices; i++) {
        if (vertex_getId(g->vertices[i]) == id) {
            for (j = 0; j < g->num_vertices; j++) {
                if (g->connections[i][j] == TRUE) {
                    connections[x] = vertex_getId(g->vertices[j]);
                    x++;
                }
            }
            break;
        }
    }

    return connections;
}

int graph_getNumberOfConnectionsFromTag(const Graph *g, char *tag) {
    int i, j, k = 0;

    if (g == NULL || tag == NULL) {
        return -1;
    }

    for (i = 0; i < g->num_vertices; i++) {
        if (strcmp(vertex_getTag(g->vertices[i]), tag) == 0) {
            for (j = 0; j < g->num_vertices; j++) {
                if (g->connections[i][j] == TRUE) {
                    k++;
                }
            }
            break;
        }
    }

    return k;
}

int graph_print(FILE *pf, const Graph *g) {
    int length, i, j;
    char c;

    if (pf==NULL || g==NULL) {
        return -1;
    }

    for (i = 0; i < g->num_vertices; i++) {
        vertex_print(pf, g->vertices[i]);
        fprintf(pf, ": ");

        for (j = 0; j < g->num_vertices; j++) {
            if (g->connections[i][j] == TRUE) {
                vertex_print(pf, g->vertices[j]);
                fprintf(pf, " ");
            }
        }

        fprintf(pf, "\n");
    }

    rewind(pf);

    length = 0;
    while (fscanf(pf, "%c", &c) == 1) {
        if (c != ' ' && c != '\n') {
            length++;
        }
    }

    return length;
}

Status graph_readFromFile(FILE *fin, Graph *g) {
    int i, j;
    long id, id1, id2;
    char tag[TAG];

    if (g == NULL || fin == NULL) {
        return ERROR;
    }

    fscanf(fin, "%d\n", &g->num_vertices);

    i = 0;
    while (fscanf(fin, "id:%ld tag:%s\n", &id, tag) == 2) {
        g->vertices[i] = vertex_init();
        vertex_setId(g->vertices[i], id);
        vertex_setTag(g->vertices[i], tag);
        i++;
    }

    while (fscanf(fin, "%ld %ld", &id1, &id2) == 2) {
        for (i = 0; i < g->num_vertices; i++) {
            if (vertex_getId(g->vertices[i]) == id1) {
                for (j = 0; j < g->num_vertices; j++) {
                    if (vertex_getId(g->vertices[j]) == id2) {
                        graph_newEdge(g, id1, id2);
                    }
                }
            }
        }
    }

    fclose(fin);
    return OK;
}
Status graph_breathSearch(Graph *g, long from_id, long to_id) {
    // 1. Verificamos que el grafo existe
    if (!g) return ERROR;
    
    // 2. Marcamos todos los vértices como NO visitados
    for (int i = 0; i < graph_getNumberOfVertices(g); i++) {
        Vertex *v = graph_getVertexByIndex(g, i);
        vertex_setState(v, 0); // 0 = NO visitado
    }
    
    // 3. Buscamos el vértice de inicio
    Vertex *start = graph_getVertexById(g, from_id);
    if (!start) return ERROR; // Si no existe, error
    
    // 4. Creamos una cola para el BFS
    Queue *q = queue_new();
    if (!q) return ERROR; // Si no se pudo crear la cola
    
    // 5. Marcamos el inicio como visitado y lo ponemos en la cola
    vertex_setState(start, 1); // 1 = Visitado
    queue_push(q, start);
    
    Bool encontrado = FALSE; // Para saber si encontramos el destino
    
    // 6. Mientras haya vértices en la cola y no hayamos encontrado el destino
    while (!queue_isEmpty(q) && !encontrado) {
        // Sacamos el primer vértice de la cola
        Vertex *actual = queue_pop(q);
        
        // 7. Verificamos si es el vértice que buscamos
        if (vertex_getId(actual) == to_id) {
            encontrado = TRUE;
            break;
        }
        
        // 8. Exploramos todos los vecinos del vértice actual
        for (int i = 0; i < vertex_getNumberOfConnections(actual); i++) {
            Vertex *vecino = vertex_getConnection(actual, i);
            
            // Si el vecino no ha sido visitado
            if (vertex_getState(vecino) == 0) {
                vertex_setState(vecino, 1); // Lo marcamos como visitado
                queue_push(q, vecino);      // Lo agregamos a la cola
            }
        }
    }
    
    // 9. Liberamos la memoria de la cola
    queue_free(q);
    
    // 10. Devolvemos OK si encontramos el destino, ERROR si no
    return encontrado ? OK : ERROR;
}
