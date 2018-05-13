#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#define N 100000

// ОДИН ПРОЦЕСС ПЕРСОНАЛЬНО ВЫСЫЛАЕТ ОПРЕДЕЛЕННЫЙ "МУСОР" ВСЕМ ПРОЦЕССАМ

int main(int argc, char*argv[])
{
    int rank = 0;
    int num_proc = 0;
    int *rbuf;
    int sbuf = 0;
    
    double result;
    double start_time = 0
    double end_time = 0;
    
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    
        sbuf = (int*)malloc(num_proc*sizeof(int));
    
        for (i = 0 ; i < N; i++)
        {
            MPI_Barrier(MPI_COMM_WORLD);
                if (rank == 0)
                {
                    
                        start_time = MPI_Wtime();
                }

                MPI_Scatter(&sbuf, 1, MPI_INT, rbuf, 1, MPI_INT, 0, MPI_COMM_WORLD);
                                        // Обобщенная передача данных от одного всем
                                // Части передающего буфера из задачи root распределяются по приемным буферам всех задач. Эта функция должна вызываться одновременно всеми процессами приложения.
                                        // sbuf - адрес начала буфера посылки
                                        // выходной параметр rbuf - адрес начала буфера сборки данных
                MPI_Barrier(MPI_COMM_WORLD);
            
                if (rank == 0)
                {
                    
                        end_time = MPI_Wtime();
                }
                result += end_time - start_time;
        }
        if (rank == 0)
        {
                printf("TIME = %lf\n", result/N);
                free(&sbuf);
        }
        MPI_Finalize();
        return 0;
}
