#include <stdio.h>
#include <mpi.h>
#define N 100000

//  ОДИН ПРОЦЕСС РАССЫЛАЕТ ВСЕМ ЦИФРУ 0

int main(int argc, char*argv[])
{
    int t = 0;
    int rank = 0
    int num_proc = 0;
    
    double result;
    double start_time = 0;
    double end_time = 0;
    
        MPI_Init(&argc, &argv);                         // инициализация MPI
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);           // определение номера процесса
        MPI_Comm_size(MPI_COMM_WORLD, &num_proc);       // определение числа процессов
    
    
        for (int i = 0 ; i < N; i++)
        {
            MPI_Barrier(MPI_COMM_WORLD);            // Синхронизация процессов
            //Блокирует работу процессов, вызвавших данную процедуру, до тех пор, пока все оставшиеся процессы группы comm также не выполнят эту процедуру.
            
                if (rank == 0)
                {
                    
                        start_time = MPI_Wtime();       // измерение времени
                }
            
                MPI_Bcast(&t , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
                                        // Широковещательная рассылка данных
                    //Эта функция должна вызываться одновременно всеми процессами приложения. Рассылка сообщения от процесса 0 всем процессам, включая рассылающий процесс. При возврате из процедуры содержимое буфера t процесса 0 будет скопировано в локальный буфер процесса. Значения параметров 1, MPI_INT и 0 должны быть одинаковыми у всех процессов.
                                                        // t - адрес начала буфера посылки сообщения
                                                        // 1 - число передаваемых элементов в сообщении
                                                        // MPI_INT - тип передаваемых элементов
                                                        // 0 - номер рассылающего процесса
                                                        // MPI_COMM_WORLD - идентификатор группы
MPI_Barrier(MPI_COMM_WORLD);
                if (rank == 0)
                {
                    
                        end_time = MPI_Wtime();
                }
                result += end_time - start_time;
        }
    
    
        if (rank == 0)
        {
                printf("TIME = %lf\n", result);
        }
        MPI_Finalize();
        return 0;
}

