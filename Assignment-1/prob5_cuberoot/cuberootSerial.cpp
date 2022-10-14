#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void cuberootSerial(int N,
		    float initialGuess,
		    float values[],
		    float output[])
{

    static const float kThreshold = 0.00001f;

    for (int i=0; i<N; i++) {
        float x = values[i] * values[i];
        float guess = initialGuess;

        float error = fabs(guess * guess * guess * x - 1.f);

        while (error > kThreshold) {
            guess = (4.f * guess - x * guess * guess * guess * guess) * (1.f/3.f);
            error = fabs(guess * guess * guess * x - 1.f);
        }

        output[i] = values[i] * guess;
    }
}
