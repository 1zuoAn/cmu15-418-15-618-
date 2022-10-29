#include <stdio.h>

#include <cuda.h>
#include <cuda_runtime.h>
#include <driver_functions.h>
#include<algorithm>
#include <thrust/scan.h>
#include <thrust/device_ptr.h>
#include <thrust/device_malloc.h>
#include <thrust/device_free.h>
#include<iostream>
#include "CycleTimer.h"

using namespace std;
extern float toBW(int bytes, float sec);


//帮助函数四舍五入到2的幂
 
static inline int nextPow2(int n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

__global__ void exclusive_scan_parallel1(int* device_data, int twod, int N){
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    index *= twod;

    if(index < N)
        device_data[index + twod-1] += device_data[index + twod/2 - 1];

}

__global__ void exclusive_scan_parallel2(int* device_data, int twod, int N){
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    if(index == 0 && twod == N){
        device_data[N-1] = 0;
    }
    __syncthreads();
    index *= twod;
    if(index < N){
        int t = device_data[index + twod/2 - 1];
        device_data[index + twod/2 - 1] = device_data[index + twod - 1];
        device_data[index + twod - 1] += t;
    }
}

void exclusive_scan(int* device_data, int length)
{

    const int threadsPerBlock = 512;
    int N = length;
    for (int twod = 1; twod < N/2; twod*=2)
    {
        int twod1 = twod*2;
        exclusive_scan_parallel1<<<(N/twod1 + threadsPerBlock - 1) / threadsPerBlock, threadsPerBlock>>>(device_data, twod1, N);
        cudaThreadSynchronize();
        // parallel_for (int i = 0; i < N; i += twod1)
        // data[i+twod1-1] += data[i+twod-1];
    }
    // // downsweep phase.
    for (int twod = N/2; twod >= 1; twod /= 2)
    {
        int twod1 = twod*2;
        exclusive_scan_parallel2<<<(N/twod1 + threadsPerBlock - 1) / threadsPerBlock, threadsPerBlock>>>(device_data, twod1, N);
        cudaThreadSynchronize();
    }


}
// void exclusive_scan_iterative(int* device_data, int length)
// {
//     int N = length;
//     // upsweep phase.
//     for (int twod = 1; twod < N; twod*=2)
//     {
//         int twod1 = twod*2;
//         exclusive_scan_parallel1<<<1, N/twod1>>>(device_data, twod1, N);
//         cudaThreadSynchronize();
//         // parallel_for (int i = 0; i < N; i += twod1)
//         // data[i+twod1-1] += data[i+twod-1];
//     }
//     device_data[N-1] = 0;
//     // downsweep phase.
//     for (int twod = N/2; twod >= 1; twod /= 2)
//     {
//         int twod1 = twod*2;
//         exclusive_scan_parallel2<<<1, N/twod1>>>(device_data, twod1, N);
//         cudaThreadSynchronize();
//         // parallel_for (int i = 0; i < N; i += twod1)
//         // {
//         //     int t = data[i+twod-1];
//         //     data[i+twod-1] = data[i+twod1-1];
//         //     data[i+twod1-1] += t;
//         // }
//     }
// }

/*  这个函数是你将要编写的代码的包装器——它将输入复制到 GPU，并且 exclusive_scan()函数的调用次数。你不应该修改它。
 */
double cudaScan(int* inarray, int* end, int* resultarray)
{
    int* device_data;
    //我们将数组的大小四舍五入到2的幂，但元素在2之后
    //初始输入的末尾未初始化，也不检查是否正确。
    //如果你假设数组的长度是2的幂，但这将导致在非2的幂输入上增加额外的工作。
    int rounded_length = nextPow2(end - inarray);
    cudaMalloc((void **)&device_data, sizeof(int) * rounded_length);

    cudaMemcpy(device_data, inarray, (end - inarray) * sizeof(int), 
               cudaMemcpyHostToDevice);

    double startTime = CycleTimer::currentSeconds();

    exclusive_scan(device_data, rounded_length);

    // Wait for any work left over to be completed.
    cudaThreadSynchronize();
    double endTime = CycleTimer::currentSeconds();
    double overallDuration = endTime - startTime;
    
    cudaMemcpy(resultarray, device_data, (end - inarray) * sizeof(int),
               cudaMemcpyDeviceToHost);
    return overallDuration;
}


// 包装 Thrust 库独有的扫描函数 如上所述，将输入复制到 GPU 上，并且只计算扫描本身的执行时间。预计您不会产生与 Thrust 版本相比具有竞争力的性能。
 
double cudaScanThrust(int* inarray, int* end, int* resultarray) {

    int length = end - inarray;
    thrust::device_ptr<int> d_input = thrust::device_malloc<int>(length);
    thrust::device_ptr<int> d_output = thrust::device_malloc<int>(length);
    
    cudaMemcpy(d_input.get(), inarray, length * sizeof(int), 
               cudaMemcpyHostToDevice);

    double startTime = CycleTimer::currentSeconds();

    thrust::exclusive_scan(d_input, d_input + length, d_output);

    cudaThreadSynchronize();
    double endTime = CycleTimer::currentSeconds();

    cudaMemcpy(resultarray, d_output.get(), length * sizeof(int),
               cudaMemcpyDeviceToHost);
    thrust::device_free(d_input);
    thrust::device_free(d_output);
    double overallDuration = endTime - startTime;
    return overallDuration;
}

__global__ void find_cuda_peaks(int *device_input, int N, int *device_output, int* device_output_length){
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    if(index < N - 1 && index > 0){
        if(device_input[index - 1] < device_input[index] && device_input[index] > device_input[index + 1]){
            device_output[index] = index;
            atomicAdd(device_output_length,1);
        }else{
            device_output[index] = 999999;
        }
    }else{
        device_output[index] = 999999;
    }
    

}


void find_peaks(int *device_input, int length, int *device_output, int *device_output_length) {
/* TODO：
     * 查找列表中大于前后元素的所有元素，
     * 将元素的索引存储到 device_result 中。
     * 返回找到的峰元素数。
     * 根据定义，元素 0 和元素 length-1 都不是峰值。
     *
     * 你的任务是实现这个功能。你可能想要
     * 利用一个或多个对 exclusive_scan() 的调用，以及
     * 额外的 CUDA 内核启动。
     * 注意：与扫描代码一样，我们确保分配的数组是一个幂
     * 大小为 2，因此您可以使用您的 Exclusive_scan 功能，如果
     * 它需要。但是，您必须确保结果
     * 给定原始长度，find_peaks 是正确的。
     */
    int threadsPerBlock = 64;

    find_cuda_peaks<<<(length + threadsPerBlock - 1) / threadsPerBlock, threadsPerBlock>>>(device_input,length,device_output,device_output_length);
    
}



/* Timing wrapper around find_peaks. You should not modify this function.
 */
double cudaFindPeaks(int *input, int length, int *output, int *output_length) {
    int *device_input;
    int *device_output;
    int *device_output_length;
    int rounded_length = length;
    cudaMalloc((void **)&device_input, rounded_length * sizeof(int));
    cudaMalloc((void **)&device_output, rounded_length * sizeof(int));
    cudaMalloc((void **)&device_output_length, 1 * sizeof(int));
    cudaMemcpy(device_output_length, output_length, 1 * sizeof(int), 
               cudaMemcpyHostToDevice);    
    cudaMemcpy(device_input, input, length * sizeof(int), 
               cudaMemcpyHostToDevice);


    double startTime = CycleTimer::currentSeconds();
    
    find_peaks(device_input, rounded_length, device_output,device_output_length);
    

    cudaThreadSynchronize();
    double endTime = CycleTimer::currentSeconds();

    cudaMemcpy(output_length, device_output_length, 1 * sizeof(int),
               cudaMemcpyDeviceToHost);

    cudaMemcpy(output, device_output, rounded_length * sizeof(int),
               cudaMemcpyDeviceToHost);
    sort(output,output+rounded_length);
    cudaFree(device_input);
    cudaFree(device_output);
    cudaFree(device_output_length);

    return endTime - startTime;
}


void printCudaInfo()
{
    // 为了好玩，只打印出机器上的一些统计数据

    int deviceCount = 0;
    cudaError_t err = cudaGetDeviceCount(&deviceCount);

    printf("---------------------------------------------------------\n");
    printf("Found %d CUDA devices\n", deviceCount);

    for (int i=0; i<deviceCount; i++)
    {
        cudaDeviceProp deviceProps;
        cudaGetDeviceProperties(&deviceProps, i);
        printf("Device %d: %s\n", i, deviceProps.name);
        printf("   SMs:        %d\n", deviceProps.multiProcessorCount);
        printf("   Global mem: %.0f MB\n",
               static_cast<float>(deviceProps.totalGlobalMem) / (1024 * 1024));
        printf("   CUDA Cap:   %d.%d\n", deviceProps.major, deviceProps.minor);
    }
    printf("---------------------------------------------------------\n"); 
}
