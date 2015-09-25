#include <stdio.h>
#include <stdlib.h>
#include "..\Misc\GC_types.h"

bool Initialized = FALSE;

void GC_init()
{
	
	Initialized = TRUE;
}