#ifndef _DREWOS_MATH_H_
#define _DREWOS_MATH_H_

#include <stdint.h>

/*
Return the natural logarithm of x, or NaN for invalid (<=1) x.
*/
double log(double x);

/*
Return the integer logarithm of x with the given base.
*/
uint32_t ilog(uint32_t base, uint32_t x);

/*
Return integer base raised to the power of exponent.
*/
uint32_t ipow(uint32_t base, uint32_t exponent);

/*
Return the absolute value of an integer.
*/
int32_t abs(int32_t x);

#endif // _DREWOS_MATH_H_
