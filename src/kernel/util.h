#ifndef _DREWOS_UTIL_H_
#define _DREWOS_UTIL_H_

#include <stdint.h>

uint16_t itoa(int x, char *buf, uint16_t bufsize);

/*
Copy n bytes from one location in memory to another location.

@param src: Pointer to source.
@param dst: Pointer to destination.
@param n: Number of bytes to be copied.
*/
void copy_memory(char *src, char *dst, uint16_t n);

#endif // _DREWOS_UTIL_H_
