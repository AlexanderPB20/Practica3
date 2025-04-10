#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "queue.h"
#include "vertex.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <graph_file> <from_id> <to_id>\n", argv[0]);
        return 0;
    }
    
    Graph *g = graph_init();
    if (!g) return 0;
    
    if (graph_readFromFile(g, argv[1]) == ERROR) {
        graph_free(g);
        return 0;
    }
    
    long from_id = atol(argv[2]);
    long to_id = atol(argv[3]);
    
    printf("Input graph:\n");
    graph_print(stdout, g);
    
    printf("\n---DFS---\n");
    printf("From vertex id: %ld\n", from_id);
    printf("To vertex id: %ld\n", to_id);
    printf("Output:\n");
    graph_depthSearch(g, from_id, to_id);
    
    printf("\n---BFS---\n");
    printf("From vertex id: %ld\n", from_id);
    printf("To vertex id: %ld\n", to_id);
    printf("Output:\n");
    graph_breathSearch(g, from_id, to_id);
    
    graph_free(g);
    return 1;
}
