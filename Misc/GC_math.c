<<<<<<< HEAD
#include <stdio.h>
#include "GC_definitions.h"

inline int abs(int value)
=======
// Returns the absolute value of an integer
static inline int abs(int value)
>>>>>>> master
{
	return value >= 0 ? value : -value;
}

// Calculates the next iteration of the Newton's method
static inline int isqrt_next_iteration(int xk, int n)
{
	int division = n / xk;
	return (xk + division) >> 1;
}

// Calculates the integer square root of a number
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

bool_t is_prime(int number) 
{
    if (number <= 1) return FALSE; // zero and one are not prime
    int i;
    for (i=2; i <= isqrt(number); i++) 
    {
        if (number % i == 0) return FALSE;
    }
    return TRUE;
}

int biggest_previous_prime(int number)
{
	bool_t x[number];
	x[1] = FALSE;
	int i;
	for (i = 2; i < number; ++i) x[i] = TRUE;
	for (i = 2; i < isqrt(number); ++i)
	{
		if (x[i])
		{
			int j, k = 0;
			for (j = i*i + k*i; j < number; ++j)
			{
				x[j] = FALSE;
				++k;
			}
		}
	}
	int previous = 2;
	for (i = 2; i < number; ++i)
	{
		if (x[i] && i <= number) previous = i;
		else if (x[i] && i > number) return previous;
	}
	return previous;
}