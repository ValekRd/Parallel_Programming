#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

#define LENGTH 1024
#define SUM_TAG 6

#define BLOCK_SIZE 32

const int BLOCKS_NUM = LENGTH/(2*BLOCK_SIZE);

struct block_answer{
	unsigned char answ0[BLOCK_SIZE];
	unsigned char answ1[BLOCK_SIZE];
	int dig0;
	int dig1;
};

struct block{
	int block_number;
	int flag;
	char my_num1[BLOCK_SIZE];
	char my_num2[BLOCK_SIZE];
	int from_me_dig0;
	int from_me_dig1;
	unsigned char my_answ0[BLOCK_SIZE];
	unsigned char my_answ1[BLOCK_SIZE];
};

void calculate(struct block* blc);

char* convert(char* s);

char* convert_back(char* s);

int main(int argc, char* argv[]){


	FILE *read_num, *write_num;
	int ProcRank, ProcNum;
	char num1_read[LENGTH+1], num2_read[LENGTH+1];
	char *num1, *num2;
	char *final_answer, answer[LENGTH/2+1];
	char buff[BLOCK_SIZE];
	struct block_answer blc_answers[BLOCKS_NUM];

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);

	struct block blc;
	const int nitems=8;
    int          blocklengths[8] = {1,1, BLOCK_SIZE, BLOCK_SIZE, 1, 1, BLOCK_SIZE, BLOCK_SIZE};
    MPI_Datatype types[8] = {MPI_INT, MPI_INT, MPI_CHAR, MPI_CHAR, MPI_INT, MPI_INT, MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR};
    MPI_Datatype mpi_block;
    MPI_Aint     offsets[8];

    offsets[0] = offsetof(struct block, block_number);
    offsets[1] = offsetof(struct block, flag);
    offsets[2] = offsetof(struct block, my_num1);
    offsets[3] = offsetof(struct block, my_num2);
    offsets[4] = offsetof(struct block, from_me_dig0);
    offsets[5] = offsetof(struct block, from_me_dig1);
    offsets[6] = offsetof(struct block, my_answ0);
    offsets[7] = offsetof(struct block, my_answ1);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_block);
    MPI_Type_commit(&mpi_block);
    
    double start_time = 0, end_time = 0;
    if(ProcRank == 0)
    	start_time = MPI_Wtime();

	if(ProcRank == 0){
		read_num = fopen("num.txt", "r");
		write_num = fopen("ans.txt", "w");
		fscanf(read_num, "%s", num1_read);
		fscanf(read_num, "%s", num2_read);
		num1 = convert(num1_read);
		num2 = convert(num2_read);
	}	

	if(ProcRank == 0){
		int blocks_send = 0;
		int blocks_recv = 0;
		for(int i = 1; i <  ProcNum; i++){
			blc.flag = 0;
			blc.block_number = blocks_send;
			for(int j = 0; j < BLOCK_SIZE; j++){
				blc.my_num1[j] = num1[blocks_send*BLOCK_SIZE + j];
				blc.my_num2[j] = num2[blocks_send*BLOCK_SIZE + j];
			}
			MPI_Send(&blc, 1, mpi_block, i, SUM_TAG, MPI_COMM_WORLD);
			blocks_send++;
		}
		while(blocks_recv < BLOCKS_NUM){
			MPI_Status Status;
			MPI_Recv(&blc, 1, mpi_block, MPI_ANY_SOURCE, SUM_TAG, MPI_COMM_WORLD, &Status);
			blocks_recv++;
			for(int i = 0; i < BLOCK_SIZE; i++){
				blc_answers[blc.block_number].answ0[i] = blc.my_answ0[i];
				blc_answers[blc.block_number].answ1[i] = blc.my_answ1[i];
			}
			blc_answers[blc.block_number].dig0 = blc.from_me_dig0;
			blc_answers[blc.block_number].dig1 = blc.from_me_dig1;
			int src = Status.MPI_SOURCE;

			if(blocks_send == BLOCKS_NUM){
				blc.flag = -1;
				MPI_Send(&blc, 1, mpi_block, src, SUM_TAG, MPI_COMM_WORLD);
			}
			else{
				blc.block_number = blocks_send;
				blc.flag = 0;
				for(int j = 0; j < BLOCK_SIZE; j++){
					blc.my_num1[j] = num1[blocks_send*BLOCK_SIZE + j];
					blc.my_num2[j] = num2[blocks_send*BLOCK_SIZE + j];
				}
				MPI_Send(&blc, 1, mpi_block, src, SUM_TAG, MPI_COMM_WORLD);
				blocks_send++;
			}
		}
	}
	else{
		while(1){
			MPI_Status Status;
			MPI_Recv(&blc, 1, mpi_block, 0, SUM_TAG, MPI_COMM_WORLD, &Status);
			if(blc.flag == -1)
				break;
			calculate(&blc);
	
			MPI_Send(&blc, 1, mpi_block, 0, SUM_TAG, MPI_COMM_WORLD);
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);

	int dig = 0;
	if(ProcRank == 0){
		for(int i = BLOCKS_NUM - 1; i >= 0; i--)
			if(dig == 0)
				for(int j = 0; j < BLOCK_SIZE; j++){
					answer[i*BLOCK_SIZE + j] = blc_answers[i].answ0[j];
					dig = blc_answers[i].dig0;
				}
			else
				for(int j = 0; j < BLOCK_SIZE; j++){
					answer[i*BLOCK_SIZE + j] = blc_answers[i].answ1[j];
					dig = blc_answers[i].dig1;
				}
	}

	if(ProcRank == 0){
		final_answer = convert_back(answer);
		final_answer[LENGTH] = '\0';

		if(dig == 1){
			fprintf(write_num, "1");

		}
		fprintf(write_num, "%s", final_answer);
	
		fclose(read_num);
		fclose(write_num);
	}	

	if(ProcRank == 0){
		end_time = MPI_Wtime();
		printf("%f\n", end_time - start_time);
	}

	MPI_Type_free(&mpi_block);

    
	MPI_Finalize();
}

void calculate(struct block* blc){
	int dig0 = 0;
	for(int i = BLOCK_SIZE - 1; i >= 0; i--){
		blc->my_answ0[i] = blc->my_num1[i] + blc->my_num2[i] + dig0;
		if(blc->my_answ0[i] >= 100){
			blc->my_answ0[i] = blc->my_answ0[i] % 100;
			dig0 = 1;
		}
		else {
			blc->my_answ0[i] = blc->my_answ0[i] = blc->my_num1[i] + blc->my_num2[i] + dig0;
			dig0 = 0;
		}
	}
	blc->from_me_dig0 = dig0;
	int dig1 = 1;
	for(int i = BLOCK_SIZE - 1; i >= 0; i--){
		if(blc->my_answ1[i] = blc->my_num1[i] + blc->my_num2[i] + dig1 >= 100){
			blc->my_answ1[i] = (blc->my_answ1[i] = blc->my_num1[i] + blc->my_num2[i] + dig1) % 100;
			dig1 = 1;
		}
		else {
			blc->my_answ1[i] = blc->my_answ1[i] = blc->my_num1[i] + blc->my_num2[i] + dig1;
			dig1 = 0;
		}
	}
	blc->from_me_dig1 = dig1;
};

char* convert(char* s){
	char *s_new = calloc(LENGTH/2, sizeof(char));
	for(int i = 0; i < LENGTH/2; i++)
		s_new[i] = (s[2*i] - '0') * 10 + (s[2*i + 1] - '0');
	return s_new;
};

char* convert_back(char* s){
	char* s_new = calloc(LENGTH + 1, sizeof(char));
	for(int i = 0; i < LENGTH/2; i++){
		s_new[2*i + 1] = s[i] % 10 + '0';
		s_new[2*i] = s[i] / 10 + '0';
	}
	return s_new;
}
