// main.c
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>   // 仍保留，若你还有别处用到 gettimeofday
#include <errno.h>

#include "mul.h"

typedef unsigned long long UINT64;

static const double TRIP_COUNT = (double)NUM * NUM * NUM;
static const int    FLOP_PER_ITERATION = 2;

static inline double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// Initialize an array with data
void init_arr(TYPE row, TYPE col, TYPE off, TYPE (*a)[NUM])
{
    for (int i = 0; i < NUM; i++) {
        for (int j = 0; j < NUM; j++) {
            a[i][j] = row * i + col * j + off;
        }
    }
}

// Print out contents of small arrays
void print_arr(const char *name, TYPE (*array)[NUM])
{
    printf("\n%s\n", name);
    for (int i = 0; i < NUM; i++){
        for (int j = 0; j < NUM; j++) {
            printf("%g\t", array[i][j]);
        }
        printf("\n");
        fflush(stdout);
    }
}

static void MultiplyOnce(int iter,
                         TYPE (*a)[NUM],
                         TYPE (*b)[NUM],
                         TYPE (*c)[NUM],
                         TYPE (*t)[NUM])
{
    // 初始化数据（如不需要每次变动，可仅首次初始化）
    init_arr(3, -2, 1, a);
    init_arr(-2, 1, 3, b);

    // 这里将 b 转置到 t，以提升访存局部性（mul.c 已配合修改）
    transpose_mat(NUM, b, t);

    double t0 = now_sec();
    ParallelMultiply(NUM, a, t, c);   // 传入转置后的 t（见 mul.h / mul.c）
    double t1 = now_sec();

    double secs  = t1 - t0;
    double flops = TRIP_COUNT * FLOP_PER_ITERATION;
    double mflops = flops / 1e6 / secs;

    printf("Matrix multiply iteration %d: %6.3f s  (%.2f MFLOPS)\n", iter, secs, mflops);
    fflush(stdout);
}

int main(void)
{
    Initialize();

    // 一次性分配并对齐
    TYPE (*a)[NUM];
    TYPE (*b)[NUM];
    TYPE (*c)[NUM];
    TYPE (*t)[NUM];
    size_t bytes = (size_t)NUM * NUM * sizeof(TYPE);

    if (posix_memalign((void **)&a, 256, bytes) ||
        posix_memalign((void **)&b, 256, bytes) ||
        posix_memalign((void **)&c, 256, bytes) ||
        posix_memalign((void **)&t, 256, bytes)) {
        perror("posix_memalign");
        return EXIT_FAILURE;
    }

    for (int i = 1; i <= 100; i++) {
        MultiplyOnce(i, a, b, c, t);
    }

    free(a);
    free(b);
    free(c);
    free(t);

    return 0;
}
