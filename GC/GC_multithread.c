#include "../Misc/GC_definitions.h"
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/select.h>
#include "../HashMap/hash_map_t.h"

/* =========== Global variables ===========*/

sem_t* mutual_exclusion_semaphore;
int pipe_fds[2];

/* =========== Local private functions =========== */

static void GC_main()
{
	while (1)
	{
		// Wait for a socket signal or the timeout


		// Signal sweep starting
		// sem_wait(mutual_exclusion_semaphore);

		// Sweep...

		// Signal sweep ended
		// sem_post(mutual_exclusion_semaphore);
	}
}

/* =========== User functions =========== */

void GC_init(int interval)
{
	// Parameter check
	if (interval >= 0 && interval < 30)
	{
		ERROR_HELPER("Invalid interval: must be greater or equal than 30 seconds")
	}

	// Calculate initial parameters
	heap_end = sbrk(0);
	stack_bottom = get_stack_pointer();

	// Create the pipe to communicate between the two threads
	if (pipe(pipe_fds) == -1)
	{
		ERROR_HELPER("Error creating the pipe");
	}

	// Semaphores initialization
	if (sem_init(mutual_exclusion_semaphore, 0, 1) == -1)
	{
		ERROR_HELPER("Error creating the semaphore");
	};

	// Allocate the hashmap
	allocation_map = hash_map_init();

	// Thread initialization and call to GC_main
	pthread_t* gc_main_thread = malloc(sizeof(pthread_t));
	pthread_create(gc_main_thread, NULL, GC_main, NULL);

	initialized = TRUE;
}

void* GC_alloc(size_t size)
{
	// Wait if there is another operation in process
	// sem_wait(mutual_exclusion_semaphore);

	// Allocation...

	// End allocation
	// sem_post(mutual_exclusion_semaphore);
	return (void*)0;
}

void* GC_realloc(void* pointer, size_t size)
{
	// Wait if there is another operation in process
	// sem_wait(mutual_exclusion_semaphore);

	// Allocation...

	// End allocation
	// sem_post(mutual_exclusion_semaphore);
	return (void*)0;
}

void GC_collect()
{
	return;
}