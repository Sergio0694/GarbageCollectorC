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