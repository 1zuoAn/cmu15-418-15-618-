#include <stdio.h>
#include <algorithm>
#include <getopt.h>

#include "CycleTimer.h"
#include "mandelbrot_ispc.h"

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

using namespace ispc;

void usage(const char* progname) {
    printf("Usage: %s [options]\n", progname);
    printf("Program Options:\n");
    printf("  -t  --tasks             Run ISPC code implementation with tasks\n");
    printf("  -v  --view <INT>        Use specified view settings (0-6)\n");
    printf("  -f  --field x0:y0:x1:y1 Specify set boundaries\n");
    printf("  -o  outfile             Specify output file\n");
    printf("  -?  --help              This message\n");
}


#define VIEWCNT 7

int main(int argc, char** argv) {

    const unsigned int width = 1250;
    // Height of the canvas
    const int height = 750;
    // Height of the actual image.  Make smaller to detect overrunning array
    const int iheight = height - 1;
    const int maxIterations = 256;
    const int numTries = 3;
    int viewIndex = 0;
    char fname[256];

    float x0 = -2.167;
    float x1 = 1.167;
    float y0 = -1;
    float y1 = 1;

    bool useTasks = false;

    // Support VIEWCNT views
    float scaleValues[VIEWCNT] = { 0.01f, 1.0f, 0.015f, 0.02f, 0.02f, 0.02f,  0.002f };
    float shiftXs[VIEWCNT]      = { 0.0f, 0.0f, -0.98f, 0.35f, 0.0f, -1.5f,  -1.4f };
    float shiftYs[VIEWCNT]      = { 0.0f, 0.0f,  0.30f, 0.05f,  0.73f, 0.0f, 0.0f };


    // parse commandline options ////////////////////////////////////////////
    int opt;
    static struct option long_options[] = {
        {"tasks", 0, 0, 't'},
        {"view",  1, 0, 'v'},
        {"help",  0, 0, '?'},
        {0 ,0, 0, 0}
    };

    bool have_file = false;
    while ((opt = getopt_long(argc, argv, "tv:f:o:?", long_options, NULL)) != EOF) {

        switch (opt) {
        case 't':
            useTasks = true;
            break;
        case 'v':
        {
            viewIndex = atoi(optarg);
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
    // end parsing of commandline options
    float scaleValue = scaleValues[viewIndex];
    float shiftX = shiftXs[viewIndex];
    float shiftY = shiftYs[viewIndex];
    scaleAndShift(x0, x1, y0, y1, scaleValue, shiftX, shiftY);


    if (have_file) {
      int * output = new int[width*height];
      if (useTasks)
	mandelbrot_ispc_withtasks(x0, y0, x1, y1, width, iheight, maxIterations, output);
      else
        mandelbrot_ispc(x0, y0, x1, y1, width, iheight, maxIterations, output);
      writePPMImage(output, width, height, fname, maxIterations);
      delete [] output;
      exit(0);
    }

    int *output_serial = new int[width*height];
    int *output_ispc = new int[width*height];
    int *output_ispc_par = new int[width*height];
    int *output_ispc_tasks = new int[width*height];

    for (unsigned int i = 0; i < width * height; ++i)
        output_serial[i] = 0;

    //
    // Run the serial implementation. Report the minimum time of three
    // runs for robust timing.
    //
    double minSerial = 1e30;
    for (int i = 0; i < numTries; ++i) {
        double startTime = CycleTimer::currentSeconds();
        mandelbrotSerial(x0, y0, x1, y1, width, iheight, 0, iheight, maxIterations, output_serial);
        double endTime = CycleTimer::currentSeconds();
        minSerial = std::min(minSerial, endTime - startTime);
    }

    printf("[mandelbrot serial]:\t\t[%.3f] ms\n", minSerial * 1000);
    sprintf(fname, "mandelbrot-%d-serial.ppm", viewIndex);
    writePPMImage(output_serial, width, height, fname, maxIterations);

    //
    // Compute the image using the ispc implementation
    //
    // Clear out the buffer
    for (unsigned int i = 0; i < width * height; ++i)
        output_ispc[i] = 0;

    double minISPC = 1e30;
    for (int i = 0; i < numTries; ++i) {
        double startTime = CycleTimer::currentSeconds();
        mandelbrot_ispc(x0, y0, x1, y1, width, iheight, maxIterations, output_ispc);
        double endTime = CycleTimer::currentSeconds();
        minISPC = std::min(minISPC, endTime - startTime);
    }

    printf("[mandelbrot ispc]:\t\t[%.3f] ms\n", minISPC * 1000);
    sprintf(fname, "mandelbrot-%d-ispc.ppm", viewIndex);
    writePPMImage(output_ispc, width, height, fname, maxIterations);


    if (! verifyResult (output_serial, output_ispc, width, height)) {
        printf ("Error : ISPC output differs from sequential output\n");

        delete[] output_serial;
        delete[] output_ispc;
        delete[] output_ispc_par;
        delete[] output_ispc_tasks;

        return 1;
    }

    //
    // Compute the image using the ispc +ILP implementation
    //
    // Clear out the buffer
    for (unsigned int i = 0; i < width * height; ++i)
        output_ispc_par[i] = 0;

    double minISPCPar = 1e30;
    for (int i = 0; i < numTries; ++i) {
        double startTime = CycleTimer::currentSeconds();
        mandelbrot_ispc_par2(x0, y0, x1, y1, width, iheight, maxIterations, output_ispc_par);
        double endTime = CycleTimer::currentSeconds();
        minISPCPar = std::min(minISPCPar, endTime - startTime);
    }

    printf("[mandelbrot ispc parallel]:\t\t[%.3f] ms\n", minISPCPar * 1000);
    sprintf(fname, "mandelbrot-%d-ispc-par.ppm", viewIndex);
    writePPMImage(output_ispc_par, width, height, fname, maxIterations);


    bool parOK = true;
    if (! verifyResult (output_serial, output_ispc_par, width, height)) {
        printf ("Error : ISPC parallel output differs from sequential output\n");
	parOK = false;
    }

    //
    // Compute the image using ISPC + tasks
    //
    // Clear out the buffer
    for (unsigned int i = 0; i < width * height; ++i) {
        output_ispc_tasks[i] = 0;
    }

    double minTaskISPC = 1e30;
    if (useTasks) {
        //
        // Tasking version of the ISPC code
        //
        for (int i = 0; i < numTries; ++i) {
            double startTime = CycleTimer::currentSeconds();
            mandelbrot_ispc_withtasks(x0, y0, x1, y1, width, iheight, maxIterations, output_ispc_tasks);
            double endTime = CycleTimer::currentSeconds();
            minTaskISPC = std::min(minTaskISPC, endTime - startTime);
        }

        printf("[mandelbrot multicore ispc]:\t[%.3f] ms\n", minTaskISPC * 1000);
	sprintf(fname, "mandelbrot-%d-task-ispc.ppm", viewIndex);
        writePPMImage(output_ispc_tasks, width, height, fname, maxIterations);

        if (! verifyResult (output_serial, output_ispc_tasks, width, height)) {
            printf ("Error : ISPC output differs from sequential output\n");
            return 1;
        }
    }

    printf("\t\t\t\t(%.2fx speedup from ISPC)\n", minSerial/minISPC);
    if (parOK)
	printf("\t\t\t\t(%.2fx speedup from ISPC+parallelism)\n", minSerial/minISPCPar);
    if (useTasks) {
        printf("\t\t\t\t(%.2fx speedup from task ISPC)\n", minSerial/minTaskISPC);
    }

    delete[] output_serial;
    delete[] output_ispc;
    delete[] output_ispc_par;
    delete[] output_ispc_tasks;


    return 0;
}
