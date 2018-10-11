#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    int rank;
    int num_proc;
    int t;
    MPI_Status Status;                                  // состояние полученных данных
                        // Тип данных MPI_Status - это структура, содержащая следующие поля: MPI_SOURCE (источник), MPI_TAG (метка), MPI_ERROR (ошибка)
    
    MPI_Init(NULL, NULL);                               // инициализация MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);               // определение номера процесса
        // MPI_COMM_WORLD - идентификатор группы. Группа "все процессы", связи в виде полного графа.
        // выходной параметр rank - номер вызывающего процесса в группе MPI_COMM_WORLD
    
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);           // определение числа процессов
                 // MPI_COMM_WORLD - группа "все процессы", связи в виде полного графа
                 // num_proc - размер группы. Здесь возвращается число процессов, которое пользователь задал при запуске программы.

    if (rank == 0)
    {
      printf("%d\n",rank);

      for (int j=1; j<num_proc; j++)
      {
        MPI_Send(&t,1,MPI_INT,j,0,MPI_COMM_WORLD );     // передача сообщения
                                    // t - адрес начала буфера посылки сообщения
                                    // 1 - число передаваемых элементов в сообщении
                                    // MPI_INT - тип передаваемых элементов
                                    // j - номер процесса получателя
                                    // 0 - метка сообщения
                                    // MPI_COMM_WORLD - идентификатор группы
          
        MPI_Recv( &t, 1,MPI_INT, j, MPI_ANY_TAG, MPI_COMM_WORLD, &Status );
                                //прием сообщения
                                // t - адрес начала буфера приема сообщения
                                // 1 - максимальное число элементов в принимаемом сообщении
                                // MPI_INT - тип элементов принимаемого сообщения
                                // j - номер процесса-отправителя
                                // MPI_ANY_TAG - метка принимаемого сообщения
                                // MPI_COMM_WORLD - идентификатор группы
                                // Status - параметры принятого сообщения
      }
    }

    if (rank != 0)
    {
      MPI_Recv( &t, 1,MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &Status );
      printf("%d\n",rank);
      MPI_Send(&t,1,MPI_INT, 0 ,0,MPI_COMM_WORLD );
    }
    MPI_Finalize();
}
