#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#define N 100
#define abs(x) (((x) > 0)? (x) : -(x))

int main(int argc, char **argv){

    int size, rank, cnt = 0;
    double a[N][N], a_pred[N][N], b[N][N], nev = 1.0, nev_p = 0,
        start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if(rank == 0){
        start_time = MPI_Wtime();
    }
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++){
            if(i == 0){
                a[i][j] = 200.0;
                a_pred[i][j] = 200.0;
            }else if(i == N-1){
                a[i][j] = 400.0;
                a_pred[i][j] = 400.0;
            }else if(j == 0){
                a[i][j] = 100.0;
                a_pred[i][j] = 100.0;
            }else if(j == N-1){
                a[i][j] = 300.0;
                a_pred[i][j] = 300.0;
            }else{
                a[i][j] = 0;
                a_pred[i][j] = 0;
            }
        }
    int p = N/size;
    int p0 = N - p*(size-1);
    int start = p*(rank-1) + p0;
    int end = p*rank + p0;
    if(rank == 0){
        start = 1;
        end = p0;
    }
    if(end == N)
        end = N-1;
    while(nev >= 0.001){
        MPI_Barrier(MPI_COMM_WORLD);
        if(rank == 0){
            cnt += 1;
            for(int i = 0; i < N; i++)
                for(int j = 0; j < N; j++)
                    b[i][j] = 0;
        }

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast((double *)a_pred, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);        //рассылка данных
                        //a_pred - адрес начала буфера посылки сообщения
                        //N*N - число передаваемых элементов в сообщении
                        // 0 - номер рассылающего процесса

        nev_p = 0.0;
        for(int i = start; i < end; i++)
            for(int j = 1; j < N-1; j++){
                a[i][j] = (a_pred[i-1][j] + a_pred[i+1][j] +
                            a_pred[i][j-1] + a_pred[i][j+1])/4;
                nev_p += abs(a[i][j] - a_pred[i][j]);
            }

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Reduce((double *)a, (double *)b, N*N, MPI_DOUBLE, MPI_SUM, 0,
                    MPI_COMM_WORLD);
        MPI_Reduce(&nev_p, &nev, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        if(rank == 0){
            for(int i = 0; i < N; i++){
                b[i][0] = 100.0;
                b[i][N-1] = 300.0;
                b[0][i] = 200.0;
                b[N-1][i] = 400.0;
            }
            for(int i = 0; i < N; i++)
                for(int j = 0; j < N; j++)
                    a_pred[i][j] = b[i][j];
        }
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(&nev, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
    if(rank == 0){
        end_time = MPI_Wtime();
		printf("%f\n", end_time - start_time);
        printf("%d\n", cnt);
    }
    MPI_Finalize();
}
