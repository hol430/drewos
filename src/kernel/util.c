#include "util.h"
#include "dmath.h"

static char hex_pfx[] = "0x";
static char bin_pfx[] = "0b";
static char oct_pfx[] = "0a";
static char pfx_default[] = "";

uint32_t get_prefix(uint8_t base, char **pfx) {
    switch (base) {
        case 2:
            *pfx = bin_pfx;
            return 2;
        case 8:
            *pfx = oct_pfx;
            return 2;
        case 16:
            *pfx = hex_pfx;
            return 2;
        default:
            *pfx = pfx_default;
            return 0;
    }
}

uint32_t _itoa(int32_t x, uint8_t base, char *buf, uint16_t bufsize) {
    // Determine number of digits in the number.
    // n digit number is approx. base^(n - 1), so log(base, x) = n - 1.
    uint32_t ndigit = 1u + ilog(base, abs(x));

    // Get the prefix string. This is "0x" for hex, "0b" for binary, etc.
    char *pfx;
    uint32_t pfx_len = get_prefix(base, (char **)&pfx);

    // Length of the output string will be the number of digits plus the prefix.
    uint32_t out_size = ndigit + pfx_len;

    // Negative numbers require an extra digit for the minus sign.
    if (x < 0) {
        out_size++;
    }

    // We actually require out_size + 1 for the NULL terminator.
    if (bufsize <= out_size) {
        return out_size;
    }

    uint32_t offset = 0;
    if (x < 0) {
        buf[0] = '-';
        offset++;
        x = -x;
    }

    for (uint8_t i = 0; i < pfx_len; i++) {
        buf[offset++] = pfx[i];
    }

    // For a 4 digit number in base 10, magnitude will be 1000.
    uint32_t magnitude = ipow(base, ndigit - 1);

    // out_size = ndigit + (1 if -ve) + pfx_len
    // offset = 0 + (1 if -ve) + pfx_len
    for (; offset < out_size; offset++) {
        // Integer division.
        int32_t digit = x / magnitude;

        // Get ascii code for the digit.
        int digit_offset = digit < 10 ? '0' : ('a' - 10);
        char code = (char)(digit + digit_offset);

        // Store this in the buffer.
        buf[offset] = code;

        // Truncate x, and reduce magnitude by an order of magnitude.
        x = x % magnitude;
        magnitude /= base;
    }

    // Add a null-terminator.
    buf[out_size] = (char)0;

    // TODO: refactor this to return the length of the string (ie out_size).
    return 0;
}

uint32_t itoa(int32_t x, char *buf, uint16_t bufsize) {
    return _itoa(x, 10, buf, bufsize);
}

uint32_t itoh(int32_t x, char *buf, uint16_t bufsize) {
    return _itoa(x, 16, buf, bufsize);
}

void copy_memory(char *src, char *dst, uint16_t n) {
    char *end = src + n;
    while (src != end) {
        *(dst++) = *(src++);
    }
}

uint64_t strlen(const char *s, uint8_t maxlen) {
    uint64_t len = 0;
    while (s && *s && len < maxlen) {
        len++;
        s++;
    }
    return len;
}
