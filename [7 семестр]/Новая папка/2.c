#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int main(int argc, char *argv[])
{
	
	int count_of_terms = atoi(argv[1]);
	int num_of_threads = atoi(argv[2]);

	omp_set_num_threads (num_of_threads);
	
	long double sum = 0;  
	#pragma omp parallel for reduction(+:sum)
	
		for (int i = 1; i < count_of_terms; i++)
		{
			sum += 1.0/i;
		}
	
	printf ("%.20Lf\n", sum);
	return 0;
}