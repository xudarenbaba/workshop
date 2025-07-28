// mul.c
#include "mul.h"

void mul_kernel(int msize, int tidx, int numt,
                TYPE (*restrict a)[NUM],
                TYPE (*restrict bT)[NUM],
                TYPE (*restrict c)[NUM])
{
    for (int i = tidx; i < msize; i += numt) {
        TYPE *restrict ci = c[i];
        TYPE *restrict ai = a[i];
        for (int k = 0; k < msize; k++) {
            TYPE aik = ai[k];
            TYPE *restrict bTk = bT[k];   // bT[k][j] == b[j][k]
            for (int j = 0; j < msize; j++) {
                ci[j] += aik * bTk[j];
            }
        }
    }
}

void transpose_mat(int msize,
                   TYPE (*restrict src)[NUM],
                   TYPE (*restrict dst)[NUM])
{
    for (int i = 0; i < msize; ++i) {
        for (int j = 0; j < msize; ++j) {
            dst[j][i] = src[i][j];
        }
    }
}
