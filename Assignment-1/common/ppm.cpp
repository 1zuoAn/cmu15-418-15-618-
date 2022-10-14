#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>

// Code to generate PPM file for Mandelbrot set
// using combination of brightness and color

// For using in documents, it helps to have a border.  Not needed otherwise
#define ADDBORDER 1

// "Normalized" data represents possible values as floats in [0.0, 1.0]
typedef  struct {
    float red;
    float green;
    float blue;
} normColor;

// For the PPM file, what 8-bit color
typedef  struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} byteColor;

// Represent possible hues as color points, and interpolate between them
static normColor blue = {0.0, 0.0, 1.0};
static normColor green = {0.0, 1.0, 0.0};
static normColor cyan = {0.0, 1.0, 1.0};
static normColor red = {1.0, 0.0, 1.0};
static normColor magenta = {1.0, 0.0, 1.0};
static normColor yellow = {1.0, 1.0, 0.0};
static normColor white = {1.0, 1.0, 1.0};

#define NCOLOR 7
static normColor colorList[NCOLOR] = { magenta, blue, cyan, green, yellow, red, white};

// Interpolation points.  Select white to only be assigned to maximum level
static float cutoff[NCOLOR+1] =  { 0.0, 0.167, 0.333, 0.500, 0.667, 0.833, 0.995, 1.0};

// Table mapping each possible level to byte-level color representation
static byteColor *colorMap = NULL;

// Normalize an integer
static float normalize(int val, int maxVal) {
    val = std::min(val, maxVal);
    return (float) val / (float) maxVal;
}

// Generate a value between 0.0 and 1.0 capturing desired brightness
// Use function that enhances lower values
static float brightness(float nval) {
    return pow(nval, 0.5f);
}

// Compute color by combining brightness with hue
static void buildColor(float nval, normColor *cp) {
    float adjust = brightness(nval);
    for (int idx = 0; idx < NCOLOR; idx++) {
	if (nval <= cutoff[idx+1]) {
	    float diff = cutoff[idx+1] - cutoff[idx];
	    float offset = nval - cutoff[idx];
	    float lwt = offset/diff;
	    float hwt = 1.0 - lwt;
	    cp->red =   adjust * (lwt * colorList[idx].red + hwt * colorList[idx+1].red);
	    cp->green = adjust * (lwt * colorList[idx].green + hwt * colorList[idx+1].green);
	    cp->blue =  adjust * (lwt * colorList[idx].blue + hwt * colorList[idx+1].blue);
	    return;
	}
    }
}

// Convert normalized value to byte
static unsigned char byteValue(float nval) {
    return (unsigned char) (nval * 255.f);
}

// Generate table of colors
static void initialize(int maxLevel) {
    if (colorMap != NULL)
	return;
    colorMap = new byteColor[maxLevel+1];
    for (int l = 0; l <= maxLevel; l++) {
	normColor c;
	c.red = c.green = c.blue = 0.0;
	float nval = normalize(l, maxLevel);
	buildColor(nval, &c);
	colorMap[l].red = byteValue(c.red);
	colorMap[l].green = byteValue(c.green);
	colorMap[l].blue = byteValue(c.blue);
    }
}

void
writePPMImage(int* data, int width, int height, const char *filename, int maxIterations)
{
    initialize(maxIterations);
    FILE *fp = fopen(filename, "wb");
    // write ppm header
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "255\n");

#if ADDBORDER
    // Convert border pixels to black
    int r,c;
    for (r = 0; r < height; r++) {
	c = 0;
	data[r * width + c] = 0;
	c = width-1;
	data[r * width + c] = 0;
    }
    for (c = 0; c < width; c++) {
	r = 0;
	data[r * width + c] = 0;
	r = height-1;
	data[r * width + c] = 0;
    }
#endif

    for (int i = 0; i < width*height; ++i) {
	unsigned char r = colorMap[data[i]].red;
	unsigned char g = colorMap[data[i]].green;
	unsigned char b = colorMap[data[i]].blue;
	fputc(r, fp); fputc(g, fp); fputc(b, fp);
    }
    fclose(fp);
    printf("Wrote image file %s\n", filename);
}
