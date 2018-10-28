#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
long double list_sum(long double* list, int N){
	long double sum = 0.0;
	for(int i = 0; i < N; i++)
		sum += list[i];
	return sum;
	}


int main(int argc, char **argv){
	int rank, i = 1;
	long double sum = 0.0;
	int N = (argc >= 2) ? atoi(argv[1]) : 1000;
	double delta = 0.0, startwtime;

	omp_set_num_threads(N);
	
	int arr_len = 1000000;
	long double *list;
	list = (long double*)malloc(N * sizeof(long double));
	
	
	startwtime = omp_get_wtime();
#pragma omp parallel  firstprivate(sum) private(rank)
{
	rank = omp_get_thread_num();	
	
	#pragma omp for  schedule(static) private(i)        //итерации равномерно распределяются по потокам, private(i) указывает, что каждый поток должен иметь свой собственный экземпляр переменной
	for(i=1; i<arr_len; i++)
		sum += (long double)1 / i;
		
	list[rank] = sum;
}
	
	delta =omp_get_wtime() - startwtime;
	printf("Sum = %.20LF computed in %.4f seconds\n", list_sum(list, N), delta);
	free(list);
	return 0x00;
}
