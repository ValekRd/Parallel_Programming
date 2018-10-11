#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

typedef unsigned int uint;

int main(int argc, char *argv[])
{
	uint count_of_terms = atoi(argv[1]);
	uint num_of_threads = atoi(argv[2]);

	omp_set_num_threads (num_of_threads);
	
	long double part_result_array  [num_of_threads]; 	
	long double last_element_array [num_of_threads];

	for (int i = 0; i < num_of_threads; ++i) {
		last_element_array [i] = 1;
		part_result_array  [i] = 0;
	}

	#pragma omp parallel for	
	for (uint i = 1; i < count_of_terms; ++i)
	{
		int tid = omp_get_thread_num();
		last_element_array [tid] /= i;
		part_result_array [tid] += last_element_array [tid];
	}

	for (uint i = 1; i < num_of_threads; ++i) {
		last_element_array [i] *= last_element_array [i-1];
		part_result_array  [i] *= last_element_array [i-1];
	}

	long double sum = 0;
	for (uint i = 0; i < num_of_threads; ++i)
		sum += part_result_array [i];

	printf ("%.20Lf\n", sum + 1);
}