#include "GC_math.h"

// Returns the absolute value of an integer
static inline int abs(int value)
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

// Checks if the given number is prime
bool_t is_prime(int number) 
{
	// 0 and 1 are not prime numbers
    if (number <= 1) return FALSE;

	// Iterate through from 0 to the integer root of the number
    int i;
    for (i=2; i <= isqrt(number); i++) 
    {
        if (number % i == 0) return FALSE;
    }
    return TRUE;
}

// Returns the biggest prime number lower or equal than the passed parameter
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
			for (j = i * i; j < number; j += k * i)
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