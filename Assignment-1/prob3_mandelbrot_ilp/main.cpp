#include <stdio.h>
#include <algorithm>
#include <getopt.h>

#include "CycleTimer.h"
#include "callibrate.h"

#include "mandelbrot.h"

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

    const unsigned int width = 1250;
    // Height of the canvas
    const int height = 750;
    // Height of the actual image.  Make smaller to detect overrunning array
    const int iheight = height - 1;
    const int maxIterations = 256;

    float x0 = -2.167;
    float x1 =  1.167;
    float y0 = -1;
    float y1 = 1;

    // Support VIEWCNT views
    float scaleValues[VIEWCNT] = { 0.01f, 1.0f, 0.015f, 0.02f, 0.02f, 0.02f,  0.002f };
    float shiftXs[VIEWCNT]      = { 0.0f, 0.0f, -0.98f, 0.35f, 0.0f, -1.5f,  -1.4f };
    float shiftYs[VIEWCNT]      = { 0.0f, 0.0f,  0.30f, 0.05f,  0.73f, 0.0f, 0.0f };

    // parse commandline options ////////////////////////////////////////////
    int opt;
    static struct option long_options[] = {
        {"view", 1, 0, 'v'},
        {"help", 0, 0, '?'},
        {0 ,0, 0, 0}
    };

    int viewIndex = 0;
    char fname[256];
    bool have_file = false;
    while ((opt = getopt_long(argc, argv, "v:f:o:?", long_options, NULL)) != EOF) {

        switch (opt) {
        case 'v':
        {
            viewIndex = atoi(optarg);
            // change view settings
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
	// In this mode, assume goal is to simply generate the output as fast as possible
	int* output = new int[width*height];
	mandelbrotParallel(par_funs[3].fun, par_funs[3].unrollCount,
			   x0, y0, x1, y1, width, iheight, 0, height, maxIterations, output);
	writePPMImage(output, width, height, fname, maxIterations);
	delete [] output;
	exit(0);
    }


    int* output_ref = new int[width*height];
    int* output_par = new int[width*height];

    //
    // Run the reference implementation.  Run the code numRuns times and
    // take the minimum to get a good estimate.
    //
    int numRuns = 3;
    memset(output_ref, 0, width * height * sizeof(int));
    double minRef = 1e30;
    long numIters = 0;
    for (int i = 0; i < numRuns; ++i) {
        double startSeconds = CycleTimer::currentSeconds();
	numIters = mandelbrotSerial(mandel_ref, x0, y0, x1, y1, width, iheight, 0, iheight, maxIterations, output_ref);
        double endSeconds = CycleTimer::currentSeconds();
        minRef = std::min(minRef, endSeconds - startSeconds);
    }
    double clock_rate = get_clockrate();
    printf("Measured clock rate = %.2f GHz\n", clock_rate);

    double ms = minRef * 1e3;
    double ns = minRef / numIters * 1e9;
    double cycs = clock_rate * ns;
    printf("[mandelbrot reference].\t\t[%.3f] ms.\tPer iteration:\t[%.3f] ns\t[%.2f] cycles\n", ms, ns, cycs);
    sprintf(fname, "mandelbrot-v%d-ref.ppm", viewIndex);
    writePPMImage(output_ref, width, height, fname, maxIterations);

    for (int j = 0; par_funs[j].unrollCount > 0; j++) {
	memset(output_par, 0, width * height * sizeof(int));
	double minParallel = 1e30;
	for (int i = 0; i < numRuns; ++i) {
	    double startSeconds = CycleTimer::currentSeconds();
	    numIters = mandelbrotParallel(par_funs[j].fun, par_funs[j].unrollCount,
				      x0, y0, x1, y1, width, iheight, 0, height, maxIterations, output_par);
	    double endSeconds = CycleTimer::currentSeconds();
	    minParallel = std::min(minParallel, endSeconds - startSeconds);
	}
	ms = minParallel * 1e3;
	ns = minParallel / numIters * 1e9;
	cycs = clock_rate * ns;
	printf("[mandelbrot %s].\t[%.3f] ms.\tPer iteration:\t[%.3f] ns\t[%.2f] cycles\tSpeedup\t[%.2f]\n",
	       par_funs[j].name, ms, ns, cycs, minRef/minParallel);
	sprintf(fname, "mandelbrot-v%d-par%d.ppm", viewIndex, par_funs[j].unrollCount);
	writePPMImage(output_par, width, height, fname, maxIterations);
	verifyResult (output_ref, output_par, width, height);

    }

    delete[] output_ref;
    delete[] output_par;

    return 0;
}
