#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

Vector *vector_create()
{
    Vector *vec = malloc(sizeof(Vector));
    vec->data = malloc(10 * sizeof(int));
    vec->size = 0;
    vec->capacity = 10;
    return vec;
}

void vector_append(Vector *vec, int value)
{
    if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, vec->capacity * sizeof(int));
    }
    vec->data[vec->size++] = value;
}

void vector_destroy(Vector *vec) {
    free(vec->data);
    free(vec);
}
