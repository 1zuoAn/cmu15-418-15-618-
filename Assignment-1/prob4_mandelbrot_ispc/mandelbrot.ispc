
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
// 处理两个点的 Mandelbrot 计算版本
// 同时提高指令级并行度
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


// ISPC 代码进行 Mandelbrot 计算
// 整个数组，使用双向并行一次处理两行
export void mandelbrot_ispc_par2(uniform float x0, uniform float y0, 
                            uniform float x1, uniform float y1,
                            uniform int width, uniform int height, 
                            uniform int maxIterations,
                            uniform int output[])
{
    uniform float dx = (x1 - x0) / width;
    uniform float dy = (y1 - y0) / height;
    uniform int j1 = (height-1)/2;
    foreach (j = 0 ... j1 , i = 0 ... width) {
            float x = x0 + i * dx;
            float x_2 = x0 + i * dx;
            float y = y0 + j * dy;
            float y_2 = y0 + (j + j1) * dy;
            int index1 = j * width + i;
            int index2 = (j + j1) * width + i;
            mandel_par2(x, y, x_2,y_2,maxIterations,output+index1,output+index2);
    }
    foreach (j = 2 * j1 ... height, i = 0 ... width) {
            float x = x0 + i * dx;
            float y = y0 + j * dy;

            int index = j * width + i;
            output[index] = mandel(x, y, maxIterations);
    }
}

// TODO: Try different block sizes
#define BLOCK_WIDTH  321 //1250
#define BLOCK_HEIGHT 187 //750

// slightly different kernel to support tasking
task void mandelbrot_ispc_task(uniform float x0, uniform float y0, 
                               uniform float x1, uniform float y1,
                               uniform int width, uniform int height,
                               uniform int maxIterations,
                               uniform int output[])
{

   // taskIndex 是 ISPC 内置的

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

    
    // TODO: 计算要启动多少个任务
    uniform int cnt1 = height % BLOCK_HEIGHT ? height/BLOCK_HEIGHT+1 : height/BLOCK_HEIGHT;
    uniform int cnt2 = width % BLOCK_WIDTH ? width/BLOCK_WIDTH+1 : width/BLOCK_WIDTH;
    uniform int taskCount = cnt1*cnt2;

    // 创建一堆任务
    launch[taskCount] mandelbrot_ispc_task(x0, y0, x1, y1,
                                         width, height,
                                         maxIterations,
                                         output); 
}
