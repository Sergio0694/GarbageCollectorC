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

/* ---------------------------------------------------------------------
*  is_prime
*  ---------------------------------------------------------------------
*  Description:
*    Checks if a given number is prime and returns TRUE, FALSE otherwise
*  Parameters:
*    value ---> The number to check */
bool_t is_prime(int number);

/* ---------------------------------------------------------------------
*  biggest_previous_prime
*  ---------------------------------------------------------------------
*  Description:
*    Returns the biggest prime number that is lower or equal to number.
*    It uses the sieve of Erathostenes to calculate the prime number
*  Parameters:
*    value ---> The maximum value of the prime number to return */
int biggest_previous_prime(int number);

/* ---------------------------------------------------------------------
*  extract_digits_array
*  ---------------------------------------------------------------------
*  Description:
*    Extract the digits of a number and returns an array with all 
*    of them from left to right
*  Parameters:
*    number ---> The number to split into single digits */
int* extract_digits_array(int number, int* size);

#endif