#include "dmath.h"

#define E 2.718281828459045235
#define PI 3.1415926535897932384626433
#define TAYLOR_ITERATIONS 20

double log(double x) {
    if (x <= 0) {
        // Return NaN for invalid x.
        return 0.0 / 0.0;
    }

    // Confine x to sensible range.
    int power_adjust = 0;
    while (x > 1.0) {
        x /= E;
        power_adjust++;
    }
    while (x < 0.25) {
        x *= E;
        power_adjust--;
    }

    // Use Taylor series to estimate the logarithm.
    double t = 0.0, z = x;
    for (int i = 1; i <= TAYLOR_ITERATIONS; i++) {
        t += z / i;
        z *= -x;
    }
    return t + power_adjust;
}

uint32_t ipow(uint32_t base, uint32_t exponent) {
    uint32_t result = 1;
    while (1) {
        if (exponent & 1) {
            result *= base;
        }
        exponent >>= 1;
        if (!exponent) {
            break;
        }
        base *= base;
    }
    return result;
}

uint32_t ilog(uint32_t base, uint32_t x) {
    uint32_t result;
    for (result = 0; x >= base; result++) {
        x /= base;
    }
    return result;
}

int32_t abs(int32_t x) {
    return x >= 0 ? x : -x;
}
