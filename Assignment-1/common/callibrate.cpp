/* Determine clock rate by timing known operation */

#include "CycleTimer.h"
#include "callibrate.h"

/* Assume that floating point sum requires 3 clock cycles */
static const int expected_cycles = FLOAT_SUM_CYCLES;
static const int nreps = 1024 * 1024 * 1024;

static volatile float faccum = 0.0;

static double go_fs(int n) {
    float result = 0.0;
    float val = random() & 0xFF;
    double start = CycleTimer::currentSeconds();
    for (int r = 0; r < n; r+=8) {
	result += val;
	result += val;
	result += val;
	result += val;
	result += val;
	result += val;
	result += val;
	result += val;
    }
    double finish = CycleTimer::currentSeconds();
    faccum = result;
    return finish - start;
}

double get_clockrate() {
    double ns = go_fs(nreps) * 1e9 / nreps;
    return expected_cycles/ns;
}
