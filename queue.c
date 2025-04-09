#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "queue.h"

Queue *queue_new() {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (!q) return NULL;
    
    q->front = q->data;
    q->rear = q->data;
    return q;
}

void queue_free(Queue *q) {
    if (!q) return;
    free(q);
}

Bool queue_isEmpty(const Queue *q) {
    if (!q) return TRUE;
    return (q->front == q->rear);
}

Status queue_push(Queue *q, void *ele) {
    if (!q || !ele) return ERROR;
    
    // Comprobar si la cola está llena
    if ((q->rear - q->data) == MAX_QUEUE - 1) {
        if (q->front == q->data) {
            return ERROR; // Cola llena
        } else {
            // Mover elementos al principio del array
            size_t size = q->rear - q->front;
            for (size_t i = 0; i < size; i++) {
                q->data[i] = q->front[i];
            }
            q->front = q->data;
            q->rear = q->front + size;
        }
    }
    
    *(q->rear) = ele;
    q->rear++;
    return OK;
}

void *queue_pop(Queue *q) {
    if (!q || queue_isEmpty(q)) return NULL;
    
    void *ele = *(q->front);
    q->front++;
    return ele;
}

void *queue_getFront(const Queue *q) {
    if (!q || queue_isEmpty(q)) return NULL;
    return *(q->front);
}

void *queue_getBack(const Queue *q) {
    if (!q || queue_isEmpty(q)) return NULL;
    return *(q->rear - 1);
}

size_t queue_size(const Queue *q) {
    if (!q) return 0;
    return (q->rear - q->front);
}

int queue_print(FILE *fp, const Queue *q, p_queue_ele_print f) {
    if (!fp || !q || !f) return -1;
    
    int chars = 0;
    void **current = q->front;
    
    while (current != q->rear) {
        chars += f(fp, *current);
        current++;
    }
    
    return chars;
}
