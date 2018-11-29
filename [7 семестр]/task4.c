#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define ISIZE 1000
#define JSIZE 1000

int main(int argc, char *argv[])
{
    double array[ISIZE][JSIZE];
    int rank, size;
    
    double start_time = 0;
    double end_time = 0;
    double result;
    
    
    MPI_Init(&argc, &argv);                         //MPI initialization
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);           //Defining the number of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size);           //Determining the number of processes
    
    start_time = MPI_Wtime();

 	if (((ISIZE * JSIZE) % size) != 0)
    {
        return 1;
    }

    if (rank == 0) 	                                //Array initialization (without parallelization)
    {
        for (int i = 0; i < ISIZE; i++)
            for (int j = 0; j < JSIZE; j++)
                array[i][j] = 10 * i + j;
    }

    size_t buf_size = ISIZE * JSIZE / size;
    double *buf = (double*)calloc(buf_size, sizeof(double));

    MPI_Scatter((void*)array, buf_size, MPI_DOUBLE, (void*)buf, 
    					  buf_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);//data transfer from one to all for processing
    
                                                //array - start address of the send buffer
                                                //buf - the address of the beginning of the buffer Assembly data
                                                // 0 - the number of the process on which the data is collected

    for (size_t i = 0; i < buf_size; ++i)
        buf[i] = sin(0.00001*buf[i]);

    MPI_Gather((void*)buf, buf_size, MPI_DOUBLE, (void*)array,
    					   buf_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);        //accept processed data
    
    end_time = MPI_Wtime();
    result += end_time - start_time;
    if (rank == 0)
    {
        printf("TIME = %lf\n", result);
    }
    
    free(buf);

    if (rank == 0) 	                            //output to file (without parallelization)
    {
        FILE *f = fopen("result.txt","w");
        for(int i = 0; i < ISIZE; i++)
        {
            for (int j = 0; j < JSIZE; j++){
                fprintf(f, "%f ", array[i][j]);
            }
            fprintf(f,"\n");
        }
        fclose(f);
    }
    
    
    
    MPI_Finalize();                             //MPI Closing
    return 0;
}
