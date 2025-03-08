#include "mul.h"

void mul(int msize, int tidx, int numt, Vector *vec, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM])
{
    int i,j,k;

    for (i = tidx; i < msize; i = i + numt) {
        for (j = 0; j < msize; j++) {
            for (k = 0; k < msize; k++) {
                c[i][j] = c[i][j] + a[i][k] * b[k][j];
            }
            vector_append(vec, c[i][j]);
        }
    }
}
