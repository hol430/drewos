#include "util.h"
#include "dmath.h"

uint16_t itoa(int x, char *buf, uint16_t bufsize) {
    // -INT_MAX requires 11 digits.

    uint16_t out_size = ilog(10, x);
    int magnitude = ipow(10, out_size);

    // n digit number is x * 10^(n - 1)
    out_size++;

    // Negative numbers require an extra digit for the minus sign.
    if (x < 0) {
        out_size++;
    }

    // We actually require out_size + 1 for the NULL terminator.
    if (bufsize <= out_size) {
        return out_size;
    }

    // 1234
    // out_size = 4
    int offset = 0;
    if (x < 0) {
        buf[0] = '-';
        offset++;
    }
    for (int i = 0; i < out_size; i++) {
        int digit = x / magnitude;
        buf[i + offset] = (char)(digit + '0');
        x = x % magnitude;
        magnitude /= 10;
    }
    buf[out_size] = (char)0;

    return 0;
}

void copy_memory(char *src, char *dst, uint16_t n) {
    for (uint16_t i = 0; i < n; i++) {
        *(dst++) = *(src++);
    }
}
