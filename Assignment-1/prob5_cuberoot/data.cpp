#include <algorithm>

// Generate random data
void initRandom(float *values, int N) {
    for (int i=0; i<N; i++)
    {
        // random input values
      values[i] = 0.f + 1.99f * static_cast<float>(rand()) / RAND_MAX;
    }
}

// Generate data that gives high relative speedup
void initGood(float *values, int N) {
    for (int i=0; i<N; i++)
    {
        // TODO: Choose data values that will yield high speedup
        values[i] = 1.9f; //无限接近2吧，好像2跑不出来 迭代次数是无穷
    }
}

// Generate data that gives low relative speedup
void initBad(float *values, int N) {
    for (int i=0; i<N; i++)
    {
        // TODO: Choose data values that will yield low speedup
        values[i] = -1.f;
    }
}

