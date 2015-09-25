#ifndef GC_MATH
#define GC_MATH

#include "GC_definitions.h"

/* ---------------------------------------------------------------------
*  isqrt
*  ---------------------------------------------------------------------
*  Description:
*    Returns the integer approximation by defect of the square root
*    of the given number, using the Newton's method
*  Parameters:
*    value ---> the number to use to calculate the square root */
int isqrt(int value);

bool_t is_prime(int number);

int biggest_previous_prime(int number);

#endif
