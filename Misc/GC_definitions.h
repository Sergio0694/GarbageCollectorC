#ifndef GC_DEFINITIONS
#define GC_DEFINITIONS

// General libraries
#include <stdio.h>
#include <stdlib.h>
#include "Math/GC_math.h"

// Displays an error message and terminates the process
#define ERROR_HELPER(error)    \
fprintf(stderr, "%s", error);  \
exit(EXIT_FAILURE);            \

// An unnamed enum used to replicate the C++ bool type
#ifndef BOOL
typedef enum { FALSE, TRUE } bool_t;
#endif

#endif
