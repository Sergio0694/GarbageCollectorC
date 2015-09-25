#ifndef GC_DEFINITIONS
#define GC_DEFINITIONS

// General libraries
include <stdio.h>
include <stdlib.h>

// Displays an error message and terminates the process
#define ERROR_HELPER(error) 
fprintf(stderr, error);    \
exit(EXIT_FAILURE);        \

typedef enum { FALSE, TRUE } bool_t;

#endif