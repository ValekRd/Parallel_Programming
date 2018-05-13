#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <string.h>

#define LENGTH 1024
#define SUM_TAG 6


struct my_block{
	size_t size;
	char* my_num1;
	char* my_num2;
	int to_me_dig;
	int from_me_dig0;
	int from_me_dig1;
	int from_me_dig_true;
	unsigned char* my_answ0;
	unsigned char* my_answ1;
	char* my_answ_true;
};

void calculate(struct my_block* blc){
	int dig0 = 0;
	for(int i = blc->size - 1; i >= 0; i--){
		blc->my_answ0[i] = blc->my_num1[i] + blc->my_num2[i] + dig0;
		if(blc->my_answ0[i] >= 100){
			blc->my_answ0[i] = blc->my_answ0[i] % 100;
			dig0 = 1;
		}
		else {
			blc->my_answ0[i] = blc->my_answ0[i] = blc->my_num1[i] + blc->my_num2[i] + dig0;
			dig0 = 0;
		}
//		printf("[%d][answ0][%d]my_num1[i] = %d , my_num2[i] = %d, my_answ0[i] = %d\n", ProcRank, i, blc->my_num1[i], blc->my_num2[i], blc->my_answ0[i]);
	}
	blc->from_me_dig0 = dig0;
	int dig1 = 1;
	for(int i = blc->size - 1; i >= 0; i--){
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


int main(int argc, char* argv[]){
	FILE *read_num, *write_num;
	int ProcRank, ProcNum;
	char num1_read[LENGTH+1], num2_read[LENGTH+1];
	char *num1, *num2;
	char *answer, *answer_norm;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);

	double start_time = 0, end_time = 0;
    if(ProcRank == 0)
    	start_time = MPI_Wtime();

	if(ProcRank == 0){
		read_num = fopen("numbers.txt", "r");
		write_num = fopen("answer.txt", "w");
		fscanf(read_num, "%s", num1_read);
		fscanf(read_num, "%s", num2_read);
		num1 = convert(num1_read);
		num2 = convert(num2_read);
	}	
	struct my_block blc;

	blc.size = LENGTH/(ProcNum * 2);
	blc.my_num1 = calloc(blc.size, sizeof(char));
	blc.my_num2 = calloc(blc.size, sizeof(char));
	blc.my_answ0 = calloc(blc.size, sizeof(char));
	blc.my_answ1 = calloc(blc.size, sizeof(char));

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Scatter(num1, blc.size, MPI_CHAR, blc.my_num1, blc.size, MPI_CHAR, 0, MPI_COMM_WORLD);
	MPI_Scatter(num2, blc.size, MPI_CHAR, blc.my_num2, blc.size, MPI_CHAR, 0, MPI_COMM_WORLD);

//	printf("Hello, I'm %d, size = %d, my_block1 = %d %d, my_block2 = %d %d\n", ProcRank, blc.size,  blc.my_num1[0], blc.my_num1[1], blc.my_num2[0], blc.my_num2[1]);

	calculate(&blc);
//	printf("calculate!\n");
	MPI_Barrier(MPI_COMM_WORLD);
//	printf("Hello, I'm %d, size = %d, answ0 = %d %d, answ1 = %d %d, dig0 = %d, dig1 = %d\n", ProcRank, blc.size,  blc.my_answ0[0], blc.my_answ0[1], blc.my_answ1[0], blc.my_answ1[1], 
//		blc.from_me_dig0, blc.from_me_dig1);


	MPI_Status Status;
	if(ProcRank == 0){
		MPI_Recv(&blc.to_me_dig, 1, MPI_INT, 1, SUM_TAG, MPI_COMM_WORLD, &Status);
		answer = calloc(LENGTH/2, sizeof(char));
		if(blc.to_me_dig == 0){
//			printf("0!\n");
			blc.my_answ_true = blc.my_answ0;
			blc.from_me_dig_true = blc.from_me_dig0;
		}
		else{
			blc.my_answ_true = blc.my_answ1;
			blc.from_me_dig_true = blc.from_me_dig1;
		}
	}
	else if(ProcRank == ProcNum - 1){
		blc.my_answ_true = blc.my_answ0;
		blc.from_me_dig_true = blc.from_me_dig0;
		MPI_Send(&blc.from_me_dig_true, 1, MPI_INT, ProcRank - 1, SUM_TAG, MPI_COMM_WORLD);
	}
	else{
		MPI_Recv(&blc.to_me_dig, 1, MPI_INT, ProcRank + 1, SUM_TAG, MPI_COMM_WORLD, &Status);
		if(blc.to_me_dig == 0){
			blc.from_me_dig_true = blc.from_me_dig0;
			blc.my_answ_true = blc.my_answ0;
		}
		else{
			blc.from_me_dig_true = blc.from_me_dig1;
			blc.my_answ_true = blc.my_answ1;
		}
		MPI_Send(&blc.from_me_dig_true, 1, MPI_INT, ProcRank - 1, SUM_TAG, MPI_COMM_WORLD);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	

	MPI_Gather(blc.my_answ_true, blc.size, MPI_CHAR, answer, blc.size, MPI_CHAR, 0, MPI_COMM_WORLD);

	if(ProcRank == 0){
		answer_norm = convert_back(answer);
		answer_norm[LENGTH] = '\0';
		if(blc.from_me_dig_true == 1){
		//	printf("1");
			fprintf(write_num, "1");
		}
		//printf("%s\n", answer_norm);
		fprintf(write_num, "%s", answer_norm);
		fclose(write_num);
		fclose(read_num);
	}	

	if(ProcRank == 0){
		end_time = MPI_Wtime();
		printf("%f\n", end_time - start_time);
	}
	free(blc.my_num1);
	free(blc.my_num2);
	free(blc.my_answ0);
	free(blc.my_answ1);
	if(ProcRank == 0){
		free(num1);
		free(num2);
		free(answer);
		free(answer_norm);
	}
	MPI_Finalize();
}
