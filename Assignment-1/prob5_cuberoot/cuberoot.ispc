
static const float kThreshold = 0.00001f; 

export void cuberoot_ispc(uniform int N,
                      uniform float initialGuess,
                      uniform float values[],
                      uniform float output[])
{
    foreach (i = 0 ... N) {
        float x = values[i] * values[i];
        float guess = initialGuess;

        float error = abs(guess * guess * guess * x - 1.f);

        while (error > kThreshold) {
            guess = (4.f * guess - x * guess * guess * guess * guess) * (1.f/3.f);
            error = abs(guess * guess * guess * x - 1.f);
        }

        output[i] = values[i] * guess;
    }
}

task void cuberoot_ispc_task(uniform int N,
                         uniform int span,
                         uniform float initialGuess,
                         uniform float values[],
                         uniform float output[])
{

    uniform int indexStart = taskIndex * span;
    uniform int indexEnd = min(N, indexStart + span);
    
    foreach (i = indexStart ... indexEnd) {
        float x = values[i] * values[i];
        float guess = initialGuess;

        float error = abs(guess * guess * guess * x - 1.f);

        while (error > kThreshold) {
            guess = (4.f * guess - x * guess * guess * guess * guess) * (1.f/3.f);
            error = abs(guess * guess * guess * x - 1.f);
        }

        output[i] = values[i] * guess;
    }
}

export void cuberoot_ispc_withtasks(uniform int N,
                                uniform float initialGuess,
                                uniform float values[],
                                uniform float output[])
{

    uniform int span = N / 64;  // 64 tasks

    launch[N/span] cuberoot_ispc_task(N, span, initialGuess, values, output);
}
