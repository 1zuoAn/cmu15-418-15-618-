#include <stdio.h>
#include <pthread.h>

// Use this code to time your threads
#include "CycleTimer.h"
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


// Mandelbrot set membershop的核心计算迭代复数c，判断其是否发散
static inline int mandel(float c_re, float c_im, int count)
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

//
// MandelbrotSerial --
//
// 计算mandelbrot集合的可视化图像。生成的数组包含在从集合中拒绝对应于像素的复数之前所需的迭代次数。
//
// * x0, y0, x1, y1描述映射到图像视口的复坐标。* width, height描述了输出图像的大小* startRow, totalRows描述了多少图像要计算
void mandelbrotSerial(
		      float x0, float y0, float x1, float y1,
		      int width, int height,
		      int startRow, int totalRows,
		      int maxIterations,
		      int output[])
{
    float dx = (x1 - x0) / width;
    float dy = (y1 - y0) / height;

    int endRow = startRow + totalRows;

    for (int j = startRow; j < endRow; j++) {
        for (int i = 0; i < width; ++i) {
            float x = x0 + i * dx;
            float y = y0 + j * dy;

            int index = (j * width + i);
            output[index] = mandel(x, y, maxIterations);
        }
    }
}


// Threading support
typedef struct {
    float x0, x1;
    float y0, y1;
    int width;
    int height;
    int maxIterations;
    int* output;
    int threadId;
    int numThreads;
} WorkerArgs;

//
// workerThreadStart --
//
// Thread entrypoint.
void* workerThreadStart(void* threadArgs) {
    double startTime = CycleTimer::currentSeconds();
    WorkerArgs* args = static_cast<WorkerArgs*>(threadArgs);
    int partnum = args->height/args->numThreads;
    if(args->threadId == args->numThreads - 1){
        int mod = args->height%args->numThreads;
        mandelbrotSerial(args->x0,args->y0,args->x1,args->y1,args->width,args->height,args->threadId*partnum,partnum+mod,args->maxIterations,args->output);
        double endTime = CycleTimer::currentSeconds();
        printf("T:%d  \t\t[%.3f] ms\n", args->threadId,(endTime - startTime) * 1000);
        return NULL;
    }
    for(int i = 0; i < args->numThreads - 1; i++){
        if(i == args->threadId){
            mandelbrotSerial(args->x0,args->y0,args->x1,args->y1,args->width,args->height,i*partnum,partnum,args->maxIterations,args->output);
        }
    
}

    double endTime = CycleTimer::currentSeconds();
    printf("T:%d  \t\t[%.3f] ms\n", args->threadId,(endTime - startTime) * 1000);
    return NULL;
}

//
// MandelbrotThread --
//
// 多线程实现mandelbrot集图像生成。通过pthreads执行的多线程。
void mandelbrotThread(
		      int numThreads,
		      float x0, float y0, float x1, float y1,
		      int width, int height,
		      int maxIterations, int output[])
{
    const static int MAX_THREADS = 32;

    if (numThreads > MAX_THREADS)
	{
	    fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
	    exit(1);
	}

    pthread_t workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];

    for (int i=0; i<numThreads; i++) {
        args[i].threadId = i;
        args[i].numThreads = numThreads;
        args[i].x0 = x0;
        args[i].x1 = x1;
        args[i].y0 = y0;
        args[i].y1 = y1;
        args[i].height = height;
        args[i].width = width;
        args[i].maxIterations = maxIterations;
        args[i].output = output;
	// 设置thread的参数
    }

    // 启动工作线程。注意，创建了numThreads-1 pthreads，主应用程序线程也被用作工作线程。

    for (int i=0; i<numThreads; i++)
        pthread_create(&workers[i], NULL, workerThreadStart, &args[i]);
    //等待工作线程完成
    for (int i=0; i<numThreads; i++)
        pthread_join(workers[i], NULL);

}
