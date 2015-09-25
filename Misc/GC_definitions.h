#ifndef GC_DEFINITIONS
#define GC_DEFINITIONS

// General libraries
#include <stdio.h>
#include <stdlib.h>
<<<<<<< HEAD

// Displays an error message and terminates the process
#define ERROR_HELPER(error)      \
fprintf(stderr, "%s", error);    \
exit(EXIT_FAILURE);        		 \
=======
#include "Math/GC_math.h"

// Displays an error message and terminates the process
#define ERROR_HELPER(error)    \
fprintf(stderr, "%s", error);  \
exit(EXIT_FAILURE);            \
>>>>>>> master

typedef enum { FALSE, TRUE } bool_t;

#endif
