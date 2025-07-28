// thrmodel.c
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "mul.h"

typedef struct tparam {
    TYPE (*a)[NUM];
    TYPE (*bT)[NUM];
    TYPE (*c)[NUM];
    int   msize;
    int   tidx;
    int   numt;
} tparam_t;

static void *ThreadFunction(void *ptr)
{
    tparam_t *par = (tparam_t *)ptr;
    assert(par->numt > 0);
    assert(par->a && par->bT && par->c);

    mul_kernel(par->msize, par->tidx, par->numt, par->a, par->bT, par->c);

    return NULL;
}

void ParallelMultiply(int msize,
                      TYPE (*restrict a)[NUM],
                      TYPE (*restrict bT)[NUM],
                      TYPE (*restrict c)[NUM])
{
    int NTHREADS = MAXTHREADS;
    int MSIZE    = NUM;

    pthread_t ht[MAXTHREADS];
    tparam_t  par[MAXTHREADS];

    GetModelParams(&NTHREADS, &MSIZE, 0);

    for (int tidx = 0; tidx < NTHREADS; tidx++) {
        par[tidx].msize = MSIZE;
        par[tidx].numt  = NTHREADS;
        par[tidx].tidx  = tidx;
        par[tidx].a     = a;
        par[tidx].bT    = bT;
        par[tidx].c     = c;

        int rc = pthread_create(&ht[tidx], NULL, ThreadFunction, &par[tidx]);
        if (rc != 0) {
            fprintf(stderr, "pthread_create failed: %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }

    for (int tidx = 0; tidx < NTHREADS; tidx++) {
        int rc = pthread_join(ht[tidx], NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_join failed: %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }
}

void GetModelParams(int *p_nthreads, int *p_msize, int print)
{
    int nthr  = MAXTHREADS;
    int msize = NUM;
    int ncpu  = getCPUCount();

    if (ncpu < MAXTHREADS)
        nthr = ncpu;

    while ((msize % nthr) != 0)
        nthr--;

    if (p_nthreads) *p_nthreads = nthr;
    if (p_msize)    *p_msize    = msize;

    if (print) {
        printf("Threads: %d\n", nthr);  fflush(stdout);
        printf("Matrix size: %d\n", msize); fflush(stdout);
    }
}
