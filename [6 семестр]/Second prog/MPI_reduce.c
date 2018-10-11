#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#define N 100000

// ОДИН ПРОЦЕСС СОБИРАЕТ СУММУ МУСОРНЫХ ЦИФР ВСЕХ ПРОЦЕССОВ

int main(int argc, char*argv[])
{
    int rank = 0
    int num_proc = 0;
    int sbuf = 0;                                   // буфер памяти с отправляемым сообщением
    int rbuf = 0;
    
    double result;
    double start_time = 0;
    double end_time = 0;
    
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    
    &rbuf = (int*)malloc(num_proc*sizeof(int));       //буфер памяти для результирующего сообщения
                                                        // (только для процесса с рангом 0)
    
        for (int i = 0 ; i < N; i++)
        {
            MPI_Barrier(MPI_COMM_WORLD);
                if (rank == 0)
                {
                    
                        start_time = MPI_Wtime();
                }

                MPI_Reduce(&sbuf, &rbuf ,1, MPI_INT, MPI_SUM, 0,  MPI_COMM_WORLD);
                                        // Сбор данных с операцией
                            // Эта функция должна вызываться одновременно всеми процессами приложения. Все процессы подают на вход функции массивы buf одинаковой размерности 1. Над этими массивами поэлементно выполняется операция MPI_SUM (суммирование). Массив - результат помещается в процесс 0.
                            // sbuf - адрес начала буфера посылки
                            // rbuf - адрес начала буфера приема

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
                free(&rbuf);
        }
        MPI_Finalize();
        return 0;
}
