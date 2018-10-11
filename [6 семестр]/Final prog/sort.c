#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

typedef struct
{
        int start;
        int end;
        int * array;
} part;

void* sort(void* ar);

int main(int argc, char** argv)
{
        int num_of_threads;
        int amount_of_numbers;
        FILE *reading_num, *writing_num;
        pthread_t* threads;
        double start_time , end_time;

        MPI_Init(&argc, &argv);

        reading_num = fopen("numbers.txt", "r");
        writing_num = fopen("answer.txt", "w");

        num_of_threads = strtol(argv[1],NULL, 0);
        fscanf(reading_num, "%d", &amount_of_numbers);

        int parity_flag = amount_of_numbers % 2;

        int *array = (int*)malloc(amount_of_numbers*sizeof(int));
        for (int i = 0; i < amount_of_numbers; i++)
                fscanf(reading_num, "%d", array + i);
        fclose(reading_num);

        int * num_of_pairs_in_thread = (int*)malloc(num_of_threads*sizeof(int));
        int num_of_pairs = amount_of_numbers / 2;


        start_time = MPI_Wtime();


        if (num_of_threads > num_of_pairs)
        {
                num_of_threads = num_of_pairs;
                for (int i = 0; i < num_of_threads; i++)
                        num_of_pairs_in_thread[i] = 1;
        }
        else
        {
                int residue = num_of_pairs % num_of_threads;
                for (int i = 0; i < residue; i++)
                        num_of_pairs_in_thread[i] =  num_of_pairs / num_of_threads  +  1;
                for (int i = residue; i < num_of_threads; i++)
                        num_of_pairs_in_thread[i] =  num_of_pairs / num_of_threads;
        }


        threads = (pthread_t*) malloc(num_of_threads*sizeof(pthread_t));
        part * parts = (part*) malloc(num_of_threads*sizeof(part));

        int index;
        for (int i = 1; i <= amount_of_numbers; i++)
        {
            if (i % 2)
            {
                index = 0;
                for (int j = 0; j < num_of_threads; j++)
                {
                    parts[j].start = index;
                    index += num_of_pairs_in_thread[j] * 2;
                    parts[j].end = index;
                    parts[j].array = array;
                    pthread_create(&(threads[j]), NULL, sort, &parts[j]);
                }
            }
            else
            {
                index = 1;
                for (int j = 0; j < num_of_threads; j++)
                {
                    parts[j].start = index;
                    if (j == num_of_threads - 1)
                        index += (num_of_pairs_in_thread[j] - 1+parity_flag) * 2;
                    else
                        index += num_of_pairs_in_thread[j] * 2;
                    parts[j].end = index;
                    parts[j].array = array;
                    pthread_create(&(threads[j]), NULL, sort, &parts[j]);
                }
            }
            for (int j = 0; j < num_of_threads; j++)
                pthread_join(threads[j], NULL);
        }

        end_time = MPI_Wtime();
        printf("TIME = %lf\n", end_time - start_time);

        for (int i = 0; i < amount_of_numbers; i++)
                fprintf(writing_num, "%d ", array[i]);

        fclose(writing_num);

        free(threads);
        free(num_of_pairs_in_thread);
        free(parts);
        free(array);
        MPI_Finalize();
}

void* sort(void * ar)
{
    int tmp;
    part * tmp_part = (part *)ar;
    for (int i = tmp_part->start; i < tmp_part->end; i+=2)
    {
        if (tmp_part->array[i] > tmp_part->array[i + 1])
        {
            tmp = tmp_part->array[i];
            tmp_part->array[i] = tmp_part->array[i + 1];
            tmp_part->array[i + 1] = tmp;
        }
    }
}

