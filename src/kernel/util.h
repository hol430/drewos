#ifndef _DREWOS_UTIL_H_
#define _DREWOS_UTIL_H_

#include <stdint.h>

uint32_t itoa(int32_t x, char *buf, uint16_t bufsize);

uint32_t itoh(int32_t x, char *buf, uint16_t bufsize);

// Measure the length of a string.
uint64_t strlen(const char *s, uint8_t maxlen);

/*
Copy n bytes from one location in memory to another location.

@param src: Pointer to source.
@param dst: Pointer to destination.
@param n: Number of bytes to be copied.
*/
void copy_memory(char *src, char *dst, uint16_t n);

#endif // _DREWOS_UTIL_H_
