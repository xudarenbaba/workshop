#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    int *data;
    int size;
    int capacity;
} Vector;

Vector *vector_create();
void vector_append(Vector *vec, int value);
void vector_destroy(Vector *vec);

#endif
