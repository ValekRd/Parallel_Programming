#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    int rank;
    int num_proc;
    int t;
    MPI_Status Status;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

    if ((rank != num_proc-1) && (rank != 0))
    {
      MPI_Recv( &t, 1,MPI_INT, rank-1, MPI_ANY_TAG, MPI_COMM_WORLD, &Status );
      printf("%d\n",rank);
      MPI_Send(&t,1,MPI_INT,rank+1,0,MPI_COMM_WORLD );

    }

    if (rank == 0)
    {
      printf("%d\n",rank);
      MPI_Send(&t,1,MPI_INT,rank+1,0,MPI_COMM_WORLD );

    }

    if (rank == num_proc-1)
    {
      MPI_Recv( &t, 1,MPI_INT, rank-1, MPI_ANY_TAG, MPI_COMM_WORLD, &Status );
      printf("%d\n",rank);
    }
    MPI_Finalize();
}
