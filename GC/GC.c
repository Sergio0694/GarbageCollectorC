#include "..\Misc\GC_definitions.h"

bool Initialized = FALSE;


void GC_init(int interval)
{
	// Parameter check
	if (interval >= 0 && interval < 30)
	{
		ERROR_HELPER("Invalid interval: must be greater or equal than 30 seconds")
	}

	// Fork and initialize GC in second process

	// Create socket to communicate between the two processes


	Initialized = TRUE;
}