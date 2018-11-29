#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv) {
    int num_threads = 0;
    int num_iter = 0;

    // two arguments: number of threads and iterations quantity
    if (argc < 3) {
        printf("Missing %d argument(s)!\n", 3 - argc);

        return 0;
    } else {
        num_threads = atoi(argv[1]);
        num_iter = atoi(argv[2]);
    }

    omp_set_num_threads(num_threads);

    // let define array of factorials
    long double factorial[num_iter + 1];
    int i = 0;

    factorial[0] = 1.0;

    // notice that loop borders are: [1, num_iter]
    for (i = 1; i < num_iter + 1; i++) {
        factorial[i] = factorial[i - 1] * i;
    }

    // here ara 'num_threads' local sums for reducing computing error
    long double loc_sum[num_threads];

    for (i = 0; i < num_threads; i++) {
        loc_sum[i] = 0.0;
    }

    #pragma omp parallel for private(i) schedule(static)
    for (i = num_iter; i >= 0; i--) {
        loc_sum[omp_get_thread_num()] += 1.0 / factorial[i];
    }

    long double sum = 0.0;

    for (i = num_threads - 1; i >= 0; i--) {
        //printf("loc_sum[%d] = %.20Lf\n", i, loc_sum[i]);
        sum += loc_sum[i];
    }

    printf("e = %.20Lf\n", sum);


    return 0;
}
