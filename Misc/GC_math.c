#include <stdio.h>

inline int abs(int value)
{
	return value >= 0 ? value : -value;
}

inline int isqrt_next_iteration(int xk, int n)
{
	int division = n / xk;
	return (xk + division) >> 1;
}

int isqrt(int value)
{
	int xk = 1, xknext = isqrt_next_iteration(xk, value);
	while (abs(xknext - xk) >= 1)
	{
		int temp = xk;
		xk = xknext;
		xknext = isqrt_next_iteration(xk, value);
	}
	return xknext;
}