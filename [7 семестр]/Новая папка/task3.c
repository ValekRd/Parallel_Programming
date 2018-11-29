#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main (int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("launch: ./task3 [N] [threads]\n");
        exit(EXIT_FAILURE);
    }
    unsigned N = atoi(argv[1]);
    unsigned n = atoi(argv[2]);
    long double sum = 0.0;
    long double *part_sum = (long double*)calloc(n, sizeof(long double));
    long double *part_fact = (long double*)calloc(n, sizeof(long double));
#pragma omp parallel num_threads(n)
    {
        unsigned t = omp_get_thread_num();
        part_sum[t] = 0.0;
        part_fact[t] = 1.0;
        for (unsigned i = N / n * t + 1; i <= N / n * (t + 1); ++i)
        {
            part_fact[t] /= i;
            part_sum[t] += part_fact[t];
        }
    }
    for (unsigned i = 1; i < n; ++i)
    {
        part_sum[i] *= part_fact[i-1];
        part_fact[i] *= part_fact[i-1];
    }
    for (unsigned i = n; i > 0; --i)
        sum += part_sum[i-1];
    printf ("ans = %.20Lf\n", sum + 1.0);
    free(part_sum);
    free(part_fact);
    return 0;
}
