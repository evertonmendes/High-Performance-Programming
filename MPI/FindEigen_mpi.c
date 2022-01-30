#include "AdjLisT.c"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <mpi.h>

void read_arguments_or_abort(int argc, char *argv[]);
double normalize_vec(int n_vertexs, double *vector);
void mat_mult_AdjList(struct Graph *graph, double *vector, double *new_vector, int *jobs, int size_jobs);
void printfvector(double *vector, int n_vertexs);
void printfIntVector(int *vector, int n_vertexs);
void cleanVector(double *vector, int n_vertexs);
double mult_pointers(double num1, double num2);
void copy_vec(double *vector, double *new_vector, int n_vertexs);

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    //numbers of processors and rank
    int nprocs, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (argc != 4)
    {
        if(rank==0){
            fprintf(stderr, "Usage: %s <number of elements> <number of arrays>\n",argv[0]);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    
    //Read Pajek File
    char *input_filename = argv[1];
    double precision;
    sscanf(argv[2], "%lf", &precision);
    char *output_filename = argv[3];
    struct File_data *file_data = ReadPajek(input_filename);

    //work division
    int complete_sections = (file_data->n_vertexs)/nprocs;
    int rest_sections = file_data->n_vertexs - nprocs*complete_sections;

    int aux_size;
    double *auxvec= NULL;

    int *indexJobs=NULL;

    if (rank==0){
        
        aux_size = complete_sections + rest_sections;
        auxvec = (double *)malloc(aux_size * sizeof(double));
        
        indexJobs= (int *)malloc((complete_sections+rest_sections)*sizeof(int));
        for (int i=0; i<complete_sections+rest_sections;  ++i){
            indexJobs[i]= i;
        }
    }else{
        aux_size = complete_sections;
        auxvec = (double *)malloc(aux_size * sizeof(double));
        
        indexJobs= (int *)malloc((complete_sections)*sizeof(int));
        for (int i=0; i<complete_sections; ++i){
            indexJobs[i]= complete_sections*rank+rest_sections + i;
        }
    }

    double *vec = (double *)malloc((file_data->n_vertexs) * sizeof(double));
    double *new_vec = (double *)malloc((file_data->n_vertexs) * sizeof(double));
    double norm_vec, new_norm_vec;

    if (rank == 0){
        for (int i = 0; i < file_data->n_vertexs; i++)
        {
            vec[i] = rand() / (RAND_MAX + 1.0);
            if (rand() / (RAND_MAX + 1.0) >= 0.5)
            {
                vec[i] *= -1;
            }
        }
        norm_vec = normalize_vec(file_data->n_vertexs, vec);
    }

    //BroadCast Random vector
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&vec[0], file_data->n_vertexs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&norm_vec, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //wait all threads arrive here
    MPI_Barrier(MPI_COMM_WORLD);

    int stop_iter = 0;
    

    struct timeval t1, t2;
    if (rank==0){
        gettimeofday(&t1, NULL);
    }
    
    while (stop_iter < 3)
    {

        if (rank == 0){
            cleanVector(new_vec, file_data->n_vertexs);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(&new_vec[0], file_data->n_vertexs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        //multiplication
        cleanVector(auxvec, aux_size);
        mat_mult_AdjList(file_data->graph, vec, auxvec, indexJobs, aux_size);
        //wait all threads arrive here
        MPI_Barrier(MPI_COMM_WORLD);

        
        if (rank == 0){
            MPI_Gather(&auxvec[0], aux_size, MPI_DOUBLE, &new_vec[0], aux_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }else{
            if (rank !=0){
                MPI_Gather(&auxvec[0], aux_size, MPI_DOUBLE, NULL, aux_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            }
            
        }
        //wait all threads arrive here
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(&new_vec[0], file_data->n_vertexs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        if (rank==0){
            new_norm_vec = normalize_vec(file_data->n_vertexs, new_vec);
        }
        //wait all threads arrive here
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(&new_vec[0], file_data->n_vertexs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        //wait all threads arrive here
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(&new_norm_vec, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        copy_vec(vec, new_vec, file_data->n_vertexs);

        if (fabs(new_norm_vec - norm_vec) / new_norm_vec < precision)
        {
            stop_iter += 1;
        }
        else
        {
            stop_iter = 0;
        }

        norm_vec = new_norm_vec;

        //wait all threads arrive here
        MPI_Barrier(MPI_COMM_WORLD);
    }



    if (rank == 0){
        gettimeofday(&t2, NULL);

        printf("It took %.17lf milliseconds.\n", (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1e6);

        FILE *output_file;
        output_file = fopen(output_filename, "w");
        fprintf(output_file, "%lf\n", new_norm_vec);
        fprintf(output_file, "%d\n", file_data->n_vertexs);

        for (int i = 0; i < file_data->n_vertexs; i++)
        {
            fprintf(output_file, "%lf\n", vec[i]);
        }
        fclose(output_file);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);

    cleanVector(new_vec, file_data->n_vertexs);
    cleanVector(auxvec, aux_size);
    mat_mult_AdjList(file_data->graph, vec, auxvec, indexJobs, aux_size);
    
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank==0){
        new_norm_vec = normalize_vec(file_data->n_vertexs, new_vec);
    }
    

    if (rank == 0){
        if (fabs(new_norm_vec - norm_vec) / new_norm_vec < precision)
        {
            printf("The Method works well\n");
        }
        else
        {
            printf("The Method don't work so well\n");
            printf("method: %.17lf precision: %.17lf\n", fabs(new_norm_vec - norm_vec) / new_norm_vec, precision);
        }

        FILE *time_record_file;
        char timefilename[100] = "time_mpi_";
        strcat(timefilename, input_filename);
        printf("%s\n", timefilename);
        time_record_file = fopen(timefilename, "a+");
        fprintf(time_record_file, "%.10lf\n", (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1e6);
        fclose(time_record_file);

    }

    MPI_Finalize();
    
    return 0;
}


double normalize_vec(int n_vertexs, double *vector)
{

    double sum_elements = 0;
    for (int i = 0; i < n_vertexs; i++)
    {
        sum_elements += pow(vector[i], 2);
    }

    for (int i = 0; i < n_vertexs; i++)
    {
        vector[i] /= sqrt(sum_elements);
    }

    return sqrt(sum_elements);
}

void mat_mult_AdjList(struct Graph *graph, double *vector, double *new_vector, int *jobs, int size_jobs)
{

    for (int i = 0; i < size_jobs; i++)
    {
        struct Node *ptr = graph->head[jobs[i]];

        if (ptr == NULL)
        {
        }
        else
        {
            while (ptr != NULL)
            {
                new_vector[i] += mult_pointers(ptr->weight, vector[ptr->dest]);
                ptr = ptr->next;
            }
        }
    }
}

void printfvector(double *vector, int n_vertexs)
{
    for (int i = 0; i < n_vertexs; i++)
    {
        printf("%lf ", vector[i]);
    }
    printf("\n");
}

void printfIntVector(int *vector, int n_vertexs)
{
    for (int i = 0; i < n_vertexs; i++)
    {
        printf("%d ", vector[i]);
    }
    printf("\n");
}

void cleanVector(double *Clean_vector, int n_vertexs)
{
    double zero = 0;
    for (int i = 0; i < n_vertexs; i++)
    {
        Clean_vector[i] = zero;
    }
}

double mult_pointers(double num1, double num2)
{
    double aux1 = num1;
    double aux2 = num2;
    double mult_value = aux1 * aux2;
    return mult_value;
}

void copy_vec(double *vector, double *new_vector, int n_vertexs)
{
    for (int i = 0; i < n_vertexs; i++)
    {
        double aux = new_vector[i];
        vector[i] = aux;
    }
}