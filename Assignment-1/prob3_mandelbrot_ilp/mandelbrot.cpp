#include <stdio.h>
#include <pthread.h>

#include "CycleTimer.h"

#include "mandelbrot.h"


/*

  15418 Spring 2012 note: This code was modified from example code
  originally provided by Intel.  To comply with Intel's open source
  licensing agreement, their copyright is retained below.

  -----------------------------------------------------------------

  Copyright (c) 2010-2011, Intel Corporation
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  * Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  * Neither the name of Intel Corporation nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


// Core computation of Mandelbrot set membershop
// Iterate complex number c to determine whether it diverges
int mandel_ref(float c_re, float c_im, int count)
{
    float z_re = c_re, z_im = c_im;
    int i;

    for (i = 0; i < count; ++i) {

        if (z_re * z_re + z_im * z_im > 4.f)
            break;

        float new_re = z_re*z_re - z_im*z_im;
        float new_im = 2.f * z_re * z_im;
        z_re = c_re + new_re;
        z_im = c_im + new_im;
    }

    return i;
}

// The following macro provides a template for generating parallel
// implementations of the mandelbrot function, able to process a
// (small) array of numbers.  When instantiated with an unrolling
// factor, GCC will unroll the loops involving the index k, yielding
// code that exploits ILP.
#define MANDEL_BODY(ufactor) {                                                 \
    const int unroll = ufactor;					               \
    float z_re[unroll], z_im[unroll];                                          \
    float new_re[unroll], new_im[unroll];			               \
    bool done[unroll];                                                         \
    int icount[unroll];                                                        \
    for (int k = 0; k < unroll; k++) {                                         \
	z_re[k] = c_re[k];                                                     \
	z_im[k] = c_im[k];                                                     \
	done[k] = false;                                                       \
	icount[k] = 0;                                                         \
    }                                                                          \
    int i;                                                                     \
    for (i = 0; i < count; ++i) {                                              \
	bool allDone = true;                                                   \
	for (int k = 0; k < unroll; k++) {                                     \
	    icount[k] = done[k] ? icount[k] : i;                               \
	    done[k] = done[k] | (z_re[k] * z_re[k] + z_im[k] * z_im[k] > 4.f); \
	    allDone = allDone & done[k];                                       \
	}                                                                      \
	if (allDone)                                                           \
	    break;                                                             \
	for (int k = 0; k < unroll; k++) {                                     \
	    new_re[k] = z_re[k] * z_re[k] - z_im[k] * z_im[k];                 \
	    new_im[k] = 2.f * z_re[k] * z_im[k];                               \
	    z_re[k] = c_re[k] + new_re[k];                                     \
	    z_im[k] = c_im[k] + new_im[k];                                     \
	}                                                                      \
    }                                                                          \
    for (int k = 0; k < unroll; k++) {                                         \
	iters[k] = done[k] ? icount[k] : i;                                    \
    }                                                                          \
}

// Versions of the code with different levels of parallelism
void mandel_par1(float *c_re, float *c_im, int count, int *iters)
MANDEL_BODY(1)

void mandel_par2(float *c_re, float *c_im, int count, int *iters)
MANDEL_BODY(2)

void mandel_par3(float *c_re, float *c_im, int count, int *iters)
MANDEL_BODY(3)

void mandel_par4(float *c_re, float *c_im, int count, int *iters)
MANDEL_BODY(4)

void mandel_par5(float *c_re, float *c_im, int count, int *iters)
MANDEL_BODY(5)

void mandel_par6(float *c_re, float *c_im, int count, int *iters)
MANDEL_BODY(6)

void mandel_par7(float *c_re, float *c_im, int count, int *iters)
MANDEL_BODY(7)

void mandel_par8(float *c_re, float *c_im, int count, int *iters)
MANDEL_BODY(8)

// Information about the different benchmarks
par_info par_funs[] =
   {{ 1, mandel_par1, "ILP parallelism x1" },
    { 2, mandel_par2, "ILP parallelism x2" },
    { 3, mandel_par3, "ILP parallelism x3" },
    { 4, mandel_par4, "ILP parallelism x4" },
    { 5, mandel_par5, "ILP parallelism x5" },
    { 6, mandel_par6, "ILP parallelism x6" },
    { 7, mandel_par7, "ILP parallelism x7" },
    { 8, mandel_par8, "ILP parallelism x8" },
    { 0, NULL, "" }
   };

//
// MandelbrotSerial --
//
// Compute an image visualizing the mandelbrot set.  The resulting
// array contains the number of iterations required before the complex
// number corresponding to a pixel could be rejected from the set.
//
// * x0, y0, x1, y1 describe the complex coordinates mapping
//   into the image viewport.
// * width, height describe the size of the output image
// * startRow, totalRows describe how much of the image to compute
//
// Returns total number of iterations across all compuations
long mandelbrotSerial(mandel_fun mfun,
		      float x0, float y0, float x1, float y1,
		      int width, int height,
		      int startRow, int totalRows,
		      int maxIterations,
		      int output[])
{
    float dx = (x1 - x0) / width;
    float dy = (y1 - y0) / height;

    int endRow = startRow + totalRows;

    long numIters = 0;

    for (int j = startRow; j < endRow; j++) {
        for (int i = 0; i < width; ++i) {
            float x = x0 + i * dx;
            float y = y0 + j * dy;

            int index = (j * width + i);
	    int iters = mfun(x, y, maxIterations);
	    numIters += iters;
	    output[index] = iters;
        }
    }
    return numIters;
}

//
// MandelbrotParallel --
//
// Compute an image visualizing the mandelbrot set.  The resulting
// array contains the number of iterations required before the complex
// number corresponding to a pixel could be rejected from the set.
//
// Uses a function that can process multiple elements at a time
// to process multiple rows on each pass
//
// * x0, y0, x1, y1 describe the complex coordinates mapping
//   into the image viewport.
// * width, height describe the size of the output image
// * startRow, totalRows describe how much of the image to compute
//
// Returns total number of iterations across all compuations
long mandelbrotParallel(mandel_par_fun mfun,
		      int unrollCount,
		      float x0, float y0, float x1, float y1,
		      int width, int height,
		      int startRow, int totalRows,
		      int maxIterations,
		      int output[])
{
    float dx = (x1 - x0) / width;
    float dy = (y1 - y0) / height;

    int endRow = startRow + totalRows;

    long numIters = 0;

    int j;

    for (j = startRow; j <= endRow-unrollCount; j+= unrollCount) {
	// Process unrollCount rows in a single pass
	float x[unrollCount];
	float y[unrollCount];
	int iters[unrollCount];
        for (int i = 0; i < width; ++i) {
	    for (int k = 0; k < unrollCount; k++) {
		x[k] = x0 + i * dx;
		y[k] = y0 + (j+k) * dy;
		iters[k] = 0;
	    }
	    mfun(x, y, maxIterations, iters);
	    for (int k = 0; k < unrollCount; k++) {
		int index = ((j+k) * width + i);
		output[index] = iters[k];
		numIters += iters[k];
	    }
        }
    }

    // Complete any leftover rows using reference implementation
    for (; j < endRow; j++) {
        for (int i = 0; i < width; ++i) {
            float x = x0 + i * dx;
            float y = y0 + j * dy;

            int index = (j * width + i);
	    int iters = mandel_ref(x, y, maxIterations);
	    numIters += iters;
	    output[index] = iters;
        }
    }

    return numIters;
}
