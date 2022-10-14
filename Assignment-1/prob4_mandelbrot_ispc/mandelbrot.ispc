
// This is the standard function for determing whether a complex
// point is in the Mandelbrot set
static inline int mandel(float c_re, float c_im, int count) {
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

// ISPC code to do the Mandelbrot computation across
// the entire array
export void mandelbrot_ispc(uniform float x0, uniform float y0, 
                            uniform float x1, uniform float y1,
                            uniform int width, uniform int height, 
                            uniform int maxIterations,
                            uniform int output[])
{
    uniform float dx = (x1 - x0) / width;
    uniform float dy = (y1 - y0) / height;

    foreach (j = 0 ... height, i = 0 ... width) {
            float x = x0 + i * dx;
            float y = y0 + j * dy;

            int index = j * width + i;
            output[index] = mandel(x, y, maxIterations);
    }
}

// A version of the Mandelbrot computation that processes two points
// simultaneously to increase the level of instruction-level parallelism
static inline void mandel_par2(float c_re0, float c_im0, float c_re1, float c_im1, int count, int *iters0, int *iters1) {
    float z_re0 = c_re0, z_im0 = c_im0;
    float z_re1 = c_re1, z_im1 = c_im1;
    int icount0 = 0;
    int icount1 = 0;
    bool done0 = false;
    bool done1 = false;
    int i;
    for (i = 0; i < count; ++i) {

        icount0 = done0 ? icount0 : i;
        icount1 = done1 ? icount1 : i;

        done0 = done0 | (z_re0*z_re0 + z_im0*z_im0 > 4.f);
        done1 = done1 | (z_re1*z_re1 + z_im1*z_im1 > 4.f);
	
	if (done0 & done1)
	    break;

        float new_re0 = z_re0*z_re0 - z_im0*z_im0;
        float new_re1 = z_re1*z_re1 - z_im1*z_im1;

        float new_im0 = 2.f * z_re0 * z_im0;
        float new_im1 = 2.f * z_re1 * z_im1;

        z_re0 = c_re0 + new_re0;
        z_re1 = c_re1 + new_re1;

        z_im0 = c_im0 + new_im0;
        z_im1 = c_im1 + new_im1;
    }
    *iters0 = done0 ? icount0 : i;
    *iters1 = done1 ? icount1 : i;
}

// ISPC code to do the Mandelbrot computation across
// an entire array, using two-way parallelism to process two rows at a time
export void mandelbrot_ispc_par2(uniform float x0, uniform float y0, 
                            uniform float x1, uniform float y1,
                            uniform int width, uniform int height, 
                            uniform int maxIterations,
                            uniform int output[])
{
    uniform float dx = (x1 - x0) / width;
    uniform float dy = (y1 - y0) / height;

    // TODO: Write ISPC code that will use function mandel_par2 to process
    // two rows on each pass.
    // You should use the foreach construct.
    // You should handle the case where the height is not a multiple
    // of 2.

}

// TODO: Try different block sizes
#define BLOCK_WIDTH  800
#define BLOCK_HEIGHT 600

// slightly different kernel to support tasking
task void mandelbrot_ispc_task(uniform float x0, uniform float y0, 
                               uniform float x1, uniform float y1,
                               uniform int width, uniform int height,
                               uniform int maxIterations,
                               uniform int output[])
{

    // taskIndex is an ISPC built-in

    uniform int bstride = (width + BLOCK_WIDTH - 1) / BLOCK_WIDTH;
    
    uniform int xstart = (taskIndex % bstride) * BLOCK_WIDTH;
    uniform int xend = min(xstart + BLOCK_WIDTH, width);
    uniform int ystart = (taskIndex / bstride) * BLOCK_HEIGHT;
    uniform int yend = min(ystart + BLOCK_HEIGHT, height);

    uniform float dx = (x1 - x0) / width;
    uniform float dy = (y1 - y0) / height;
    
    foreach (j = ystart ... yend, i = xstart ... xend) {
            float x = x0 + i * dx;
            float y = y0 + j * dy;
            
            int index = j * width + i;
            output[index] = mandel(x, y, maxIterations);
    }
}

export void mandelbrot_ispc_withtasks(uniform float x0, uniform float y0,
                                      uniform float x1, uniform float y1,
                                      uniform int width, uniform int height,
                                      uniform int maxIterations,
                                      uniform int output[])
{

    // TODO: Figure out how many tasks to launch
    uniform int taskCount = 4;

    // create a bunch of tasks
    launch[taskCount] mandelbrot_ispc_task(x0, y0, x1, y1,
                                         width, height,
                                         maxIterations,
                                         output); 
}
