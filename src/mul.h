// mul.h
#ifndef MUL_H
#define MUL_H

#include <stddef.h>

#define MAXTHREADS 16
#define NUM        2048

typedef double TYPE;

// 工具函数
void Initialize(void);
int  getCPUCount(void);
double getCPUFreq(void);

// 计算相关
void GetModelParams(int *nthreads, int *msize, int print);

// bT 是 b 的转置
void ParallelMultiply(int msize,
                      TYPE (*restrict a)[NUM],
                      TYPE (*restrict bT)[NUM],
                      TYPE (*restrict c)[NUM]);

// 线程内核
void mul_kernel(int msize, int tidx, int numt,
                TYPE (*restrict a)[NUM],
                TYPE (*restrict bT)[NUM],
                TYPE (*restrict c)[NUM]);

// 转置
void transpose_mat(int msize,
                   TYPE (*restrict src)[NUM],
                   TYPE (*restrict dst)[NUM]);

#endif
