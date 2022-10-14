#ifndef MANDEL_H
#include <string>

typedef int (*mandel_fun)(float, float, int);

int mandel_ref(float c_re, float c_im, int count);

long mandelbrotSerial(mandel_fun f,
		      float x0, float y0, float x1, float y1,
		      int width, int height,
		      int startRow, int totalRows,
		      int maxIterations,
		      int output[]);

typedef void (*mandel_par_fun) (float *, float *, int, int *);

typedef struct {
    int unrollCount;
    mandel_par_fun fun;
    const char *name;
} par_info;

extern par_info par_funs[];

long mandelbrotParallel(mandel_par_fun f,
		    int unrollCount,
		    float x0, float y0, float x1, float y1,
		    int width, int height,
		    int startRow, int totalRows,
		    int maxIterations,
		    int output[]);


#define MANDEL_H
#endif
