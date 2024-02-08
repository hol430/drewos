#ifndef _DREWOS_MATH_H_
#define _DREWOS_MATH_H_

/*
Return the natural logarithm of x, or NaN for invalid (<=1) x.
*/
double log(double x);

/*
Return the integer logarithm of x with the given base.
*/
int ilog(int base, int x);

/*
Return integer base raised to the power of exponent.
*/
int ipow(int base, int exponent);

#endif // _DREWOS_MATH_H_
