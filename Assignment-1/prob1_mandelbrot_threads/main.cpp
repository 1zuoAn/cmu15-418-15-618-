#include <stdio.h>
#include <algorithm>
#include <getopt.h>

#include "CycleTimer.h"

extern void mandelbrotSerial(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int numRows,
    int maxIterations,
    int output[]);

extern void mandelbrotThread(
    int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations,
    int output[]);

extern void writePPMImage(
    int* data,
    int width, int height,
    const char *filename,
    int maxIterations);

void
scaleAndShift(float& x0, float& x1, float& y0, float& y1,
              float scale,
              float shiftX, float shiftY)
{

    x0 *= scale;
    x1 *= scale;
    y0 *= scale;
    y1 *= scale;
    x0 += shiftX;
    x1 += shiftX;
    y0 += shiftY;
    y1 += shiftY;

}

void usage(const char* progname) {
    printf("Usage: %s [options]\n", progname);
    printf("Program Options:\n");
    printf("  -t  --threads <N>       Use N threads\n");
    printf("  -v  --view <INT>        Use specified view settings (0-6)\n");
    printf("  -f  --field x0:y0:x1:y1 Specify set boundaries\n");
    printf("  -o  outfile             Specify output file\n");
    printf("  -?  --help              This message\n");
}

bool verifyResult (int *gold, int *result, int width, int height) {
    int i, j;
    int errLimit = 5;
    bool ok = true;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            if (gold[i * width + j] != result[i * width + j]) {
                printf ("Mismatch : [%d][%d], Expected : %d, Actual : %d\n",
                            i, j, gold[i * width + j], result[i * width + j]);
		ok = false;
		if (--errLimit <= 0) {
		    printf(" ...\n");
		    return ok;
		}
            }
        }
    }

    return ok;
}

#define VIEWCNT 7

int main(int argc, char** argv) {

    const int width = 600;
    // 画布的高度
    const int height = 600;
    // 实际图像的高度。使其变小以检测超出的数组
    const int iheight = height - 1;
    const int maxIterations = 256;
    int numThreads = 2;

    float x0 = -2.167;
    float x1 =  1.167;
    float y0 = -1;
    float y1 = 1;

    // 支持VIEWCNT视图
    float scaleValues[VIEWCNT] = { 0.01f, 1.0f, 0.015f, 0.02f, 0.02f, 0.02f,  0.002f };
    float shiftXs[VIEWCNT]      = { 0.0f, 0.0f, -0.98f, 0.35f, 0.0f, -1.5f,  -1.4f };
    float shiftYs[VIEWCNT]      = { 0.0f, 0.0f,  0.30f, 0.05f,  0.73f, 0.0f, 0.0f };

    // 解析命令行选项 ////////////////////////////////////////////
    int opt;
    static struct option long_options[] = {
        {"threads", 1, 0, 't'},
        {"view", 1, 0, 'v'},
	{"field", 1, 0, 'f'},
	{"out", 1, 0, 'o'},
        {"help", 0, 0, '?'},
        {0 ,0, 0, 0}
    };

    int viewIndex = 0;
    char fname[256];
    bool have_file = false;
    while ((opt = getopt_long(argc, argv, "t:v:f:o:?", long_options, NULL)) != EOF) {

        switch (opt) {
        case 't':
        {
            numThreads = atoi(optarg);
            break;
        }
        case 'v':
        {
            viewIndex = atoi(optarg);
            // 更改视图设置
            if (viewIndex < 0 || viewIndex >= VIEWCNT)  {
                fprintf(stderr, "Invalid view index %d\n", viewIndex);
                return 1;
            }
            break;
        }
	case 'f':
	{
	    if (sscanf(optarg, "%f:%f:%f:%f", &x0, &y0, &x1, &y1) != 4) {
		fprintf(stderr, "Couldn't extract field from '%s'\n", optarg);
		exit(1);
	    }
	    break;
	}
	case 'o':
	{
	    strcpy(fname, optarg);
	    have_file = true;
	    break;
	}
        case '?':
        default:
            usage(argv[0]);
            return 1;
        }
    }
    // 结束命令行选项的解析
    float scaleValue = scaleValues[viewIndex];
    float shiftX = shiftXs[viewIndex];
    float shiftY = shiftYs[viewIndex];
    scaleAndShift(x0, x1, y0, y1, scaleValue, shiftX, shiftY);

    if (have_file) {
	// 在这种模式下，假设目标只是尽可能快地生成输出
	int* output = new int[width*height];
        mandelbrotThread(numThreads, x0, y0, x1, y1, width, iheight, maxIterations, output);
	writePPMImage(output, width, height, fname, maxIterations);
	delete [] output;
	exit(0);
    }


    int* output_serial = new int[width*height];
    int* output_thread = new int[width*height];

    //
    // 运行串行实现。运行代码numRuns times和 启动工作线程。注意，创建了numThreads-1 pthreads，主应用程序线程也被用作工作线程。运行串行实现。运行代码numRuns times并取最小值以得到一个较好的估计。
    //
    int numRuns = 3;
    memset(output_serial, 0, width * height * sizeof(int));
    double minSerial = 1e30;
    for (int i = 0; i < numRuns; ++i) {
        double startTime = CycleTimer::currentSeconds();
        mandelbrotSerial(x0, y0, x1, y1, width, iheight, 0, iheight, maxIterations, output_serial);
        double endTime = CycleTimer::currentSeconds();
        minSerial = std::min(minSerial, endTime - startTime);
    }

    printf("[mandelbrot serial]:\t\t[%.3f] ms\n", minSerial * 1000);
    sprintf(fname, "mandelbrot-v%d-serial.ppm", viewIndex);
    writePPMImage(output_serial, width, height, fname, maxIterations);

    //
    // 运行线程版本
    //
    memset(output_thread, 0, width * height * sizeof(int));
    double minThread = 1e30;
    for (int i = 0; i < numRuns; ++i) {
        double startTime = CycleTimer::currentSeconds();
        mandelbrotThread(numThreads, x0, y0, x1, y1, width, iheight, maxIterations, output_thread);
        double endTime = CycleTimer::currentSeconds();
        minThread = std::min(minThread, endTime - startTime);
    }

    printf("[mandelbrot thread]:\t\t[%.3f] ms\n", minThread * 1000);
    sprintf(fname, "mandelbrot-v%d-thread-%d.ppm", viewIndex, numThreads);
    writePPMImage(output_thread, width, height, fname, maxIterations);

    if (! verifyResult (output_serial, output_thread, width, height)) {
        printf ("ERROR : Output from threads does not match serial output\n");

        delete[] output_serial;
        delete[] output_thread;

        return 1;
    }

    //计算加速
    printf("++++\t\t\t\t(%.2fx speedup from %d threads)\n", minSerial/minThread, numThreads);

    delete[] output_serial;
    delete[] output_thread;

    return 0;
}
