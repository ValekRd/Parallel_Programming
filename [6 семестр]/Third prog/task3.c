#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <string.h>

#define LENGTH 1024
#define SUM_TAG 6


struct block{
	size_t size;
	char* array_num1;
	char* array_num2;
	int to_rank;
	int from_rank0;
	int from_rank1;
	int from_rank_real;
	unsigned char* answer0;
	unsigned char* answer1;
	char* my_answ_real;
};

void counting(struct block* blc);
char* convert(char* s);
char* convert_back(char* s);


int main(int argc, char* argv[]){
	FILE *reading_num, *writing_num;
	int ProcRank, ProcNum;
	char num1_read[LENGTH+1], num2_read[LENGTH+1];
	char *num1, *num2;
	char *answer, *final_answer;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);

	double start_time = 0, end_time = 0;
    if(ProcRank == 0)
    	start_time = MPI_Wtime();

	if(ProcRank == 0){
		reading_num = fopen("numbers.txt", "r");
		writing_num = fopen("answer.txt", "w");
		fscanf(reading_num, "%s", num1_read);
		fscanf(reading_num, "%s", num2_read);

		num1 = convert(num1_read);
		num2 = convert(num2_read);
		}
    
	struct block blc;
	blc.size = LENGTH/(ProcNum * 2);
	blc.array_num1 = calloc(blc.size, sizeof(char));
	blc.array_num2 = calloc(blc.size, sizeof(char));
	blc.answer0 = calloc(blc.size, sizeof(char));
	blc.answer1 = calloc(blc.size, sizeof(char));

	MPI_Barrier(MPI_COMM_WORLD);
    
	MPI_Scatter(num1, blc.size, MPI_INT, blc.array_num1, blc.size, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(num2, blc.size, MPI_INT, blc.array_num2, blc.size, MPI_INT, 0, MPI_COMM_WORLD);


	counting(&blc);

	MPI_Barrier(MPI_COMM_WORLD);


	MPI_Status Status;
	if(ProcRank == 0){
		MPI_Recv(&blc.to_rank, 1, MPI_INT, 1, SUM_TAG, MPI_COMM_WORLD, &Status);
		answer = calloc(LENGTH/2, sizeof(char));
		if(blc.to_rank == 0){

			blc.my_answ_real = blc.answer0;
			blc.from_rank_real = blc.from_rank0;
		}
		else{
			blc.my_answ_real = blc.answer1;
			blc.from_rank_real = blc.from_rank1;
		}
	}
	else if(ProcRank == ProcNum - 1){
		blc.my_answ_real = blc.answer0;
		blc.from_rank_real = blc.from_rank0;
		MPI_Send(&blc.from_rank_real, 1, MPI_INT, ProcRank - 1, SUM_TAG, MPI_COMM_WORLD);
	}
	else{
		MPI_Recv(&blc.to_rank, 1, MPI_INT, ProcRank + 1, SUM_TAG, MPI_COMM_WORLD, &Status);
		if(blc.to_rank == 0){
			blc.from_rank_real = blc.from_rank0;
			blc.my_answ_real = blc.answer0;
		}
		else{
			blc.from_rank_real = blc.from_rank1;
			blc.my_answ_real = blc.answer1;
		}
		MPI_Send(&blc.from_rank_real, 1, MPI_INT, ProcRank - 1, SUM_TAG, MPI_COMM_WORLD);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	

	MPI_Gather(blc.my_answ_real, blc.size, MPI_INT, answer, blc.size, MPI_INT, 0, MPI_COMM_WORLD);     // сбор всех сумм в одну

	if(ProcRank == 0){
		final_answer = convert_back(answer);
		final_answer[LENGTH] = '\0';
		if(blc.from_rank_real == 1){
	
			fprintf(writing_num, "1");
		}
	
		fprintf(writing_num, "%s\n", final_answer);
		fclose(writing_num);
		fclose(reading_num);
	}	

	if(ProcRank == 0){
		end_time = MPI_Wtime();
		printf("%f\n", end_time - start_time);
	}
    
    free(blc.array_num1);
    free(blc.array_num2);
    free(blc.answer0);
    free(blc.answer1);
    if(ProcRank == 0){
        free(answer);
        free(num1);
        free(num2);
        free(final_answer);
    }

	MPI_Finalize();
}

void counting(struct block* blc){
    int rank0 = 0;
    for(int i = blc->size - 1; i >= 0; i--){
        blc->answer0[i] = blc->array_num1[i] + blc->array_num2[i] + rank0;
        if(blc->answer0[i] >= 100){
            blc->answer0[i] = blc->answer0[i] % 100;
            rank0 = 1;
        }
        else {
            rank0 = 0;
        }
    }
    blc->from_rank0 = rank0;
    int rank1 = 1;
    for(int i = blc->size - 1; i >= 0; i--){
        if(blc->answer1[i] = blc->array_num1[i] + blc->array_num2[i] + rank1 >= 100){
            blc->answer1[i] = (blc->answer1[i] = blc->array_num1[i] + blc->array_num2[i] + rank1) % 100;
            rank1 = 1;
        }
        else {
            blc->answer1[i] = blc->answer1[i] = blc->array_num1[i] + blc->array_num2[i] + rank1;
            rank1 = 0;
        }
    }
    blc->from_rank1 = rank1;
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
