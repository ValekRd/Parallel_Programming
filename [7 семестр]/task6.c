#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define ISIZE 20
#define JSIZE 9


int main(int argc, char **argv)
{
	int rank = 0, size = 0;
	double a[ISIZE][JSIZE];
	double b[ISIZE][JSIZE];
	double b_recv[ISIZE][JSIZE];
	int i, j;
//	FILE *ff;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int k = JSIZE/size;
	int k_first = JSIZE - k*(size-1);
	int kp = ISIZE/8;
	int kp_last = ISIZE - kp*ISIZE/8;
	int kp_n = kp;
	if(kp_last > 0)
		kp_n += 1;
	for (int i=0; i<ISIZE; i++)
		for (int j=0; j<JSIZE; j++)
			b[i][j] = 0;
	if(rank == 0){
		printf("%d %d\n", k, k_first);
		for (i=0; i<ISIZE; i++)
			for (j=0; j<JSIZE; j++)
				a[i][j] = 10*i +j;
		for(i=0; i < ISIZE; i++){
			for (j=0; j < JSIZE; j++){
				printf("%f ",a[i][j]);
			}
			printf("\n");
		}
		printf("\n");

		for(int ii = 0; ii < kp_n; ii++){
			for(int i = 1; i < size; i++)
				MPI_Send((double*)(a), JSIZE*ISIZE, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);

			int i_end = 0;
			if((ii+1)*8 > ISIZE)
				i_end = ISIZE;
			else
				i_end = (ii+1)*8;

			int j_end = k_first;
			for(int i = 8*ii; i < i_end; i++)
				for(int j = 0; j < j_end; j++)
					if(i < 8 || j >= JSIZE - 3)
						b[i][j] += a[i][j];
					else
						b[i][j] += sin(0.00001*a[i-8][j+3]);

			MPI_Status Status;
			for(int i = 1; i < size; i++){
				MPI_Recv((double*)(b_recv), JSIZE*ISIZE, MPI_DOUBLE, i, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
				for(int i = 0; i < ISIZE; i++)
					for(int j = 0; j < JSIZE; j++)
						b[i][j] += b_recv[i][j];
			}
			for(int i = 8*ii; i < i_end; i++)
				for(int j = 0; j < JSIZE; j++)
					a[i][j] = b[i][j];
		}
	}
	if(rank != 0){
		MPI_Status Status;
		for(int ii = 0; ii < kp_n; ii++){
			MPI_Recv((double*)a, JSIZE*ISIZE , MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
			int j_start = k_first+(rank-1)*k;
			int j_end = k_first+rank*k;
			int i_end = 0;
			if((ii+1)*8 > ISIZE)
				i_end = ISIZE;
			else
				i_end = (ii+1)*8;

			for(int i = 8*ii; i < i_end; i++)
				for(int j = j_start; j < j_end; j++)
					if(i < 8 || j >= JSIZE - 3)
						b[i][j] = a[i][j];
					else
						b[i][j] += sin(0.00001*a[i-8][j+3]);
			MPI_Send((double*)b, JSIZE*ISIZE, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		}
	}
	if(rank == 0){
		for(i=0; i < ISIZE; i++){
			for (j=0; j < JSIZE; j++){
				printf("%.10f ",a[i][j]);
			}
			printf("\n");
		}
	}
//	ff = fopen("result_1.txt","w");
//	for(i=0; i < ISIZE; i++){
//		for (j=0; j < JSIZE; j++){
//			fprintf(ff,"%f ",a[i][j]);
//		}
//		fprintf(ff,"\n");
//	}
//	fclose(ff);
	MPI_Finalize();
}