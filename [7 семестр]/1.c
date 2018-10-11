#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[])
{	
	int i = 0;
	int count_threads = atoi (argv[1]);

	omp_set_num_threads(count_threads);
	omp_lock_t array_lock[count_threads];

	for (i = 0; i < count_threads; i++)
		omp_init_lock(array_lock + i);
	
	for (i = 1; i < count_threads; i++)
	omp_set_lock(array_lock + i);

	#pragma omp parallel
	{	
		int thread_id = omp_get_thread_num();

		if (thread_id) {
			printf ("I am %d and ready\n", thread_id);
			omp_set_lock(array_lock + thread_id);
		}

		printf ("[I am thread number %d\n]", thread_id);
		if(thread_id < count_threads - 1)
			omp_unset_lock (array_lock + thread_id + 1);
	}
	return 0;
}
