#include "AdjLisT.c"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

void read_arguments_or_abort(int argc, char *argv[]);
double normalize_vec(int n_vertexs, double *vector);
void mat_mult_AdjList(struct Graph *graph, double *vector, double *new_vector, int n_vertexs);
void printfvector(double *vector, int n_vertexs);
void cleanVector(double *vector, int n_vertexs);
double mult_pointers(double num1, double num2);
void copy_vec(double *vector, double *new_vector, int n_vertexs);

int main(int argc, char *argv[])
{

    read_arguments_or_abort(argc, argv);
    char *input_filename = argv[1];

    double precision;
    sscanf(argv[2], "%lf", &precision);

    char *output_filename = argv[3];

    struct File_data *file_data = ReadPajek(input_filename);

    double *vec = (double *)malloc(file_data->n_vertexs * sizeof(double));
    double *new_vec = (double *)malloc(file_data->n_vertexs * sizeof(double));

    for (int i = 0; i < file_data->n_vertexs; i++)
    {
        vec[i] = rand() / (RAND_MAX + 1.0);
        if (rand() / (RAND_MAX + 1.0) >= 0.5)
        {
            vec[i] *= -1;
        }
    }

    int stop_iter = 0;
    double norm_vec, new_norm_vec;

    norm_vec = normalize_vec(file_data->n_vertexs, vec);

    struct timeval t1, t2;
    gettimeofday(&t1, NULL);
    while (stop_iter < 3)
    {

        cleanVector(new_vec, file_data->n_vertexs);
        mat_mult_AdjList(file_data->graph, vec, new_vec, file_data->n_vertexs);
        new_norm_vec = normalize_vec(file_data->n_vertexs, new_vec);
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
    }
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

    cleanVector(new_vec, file_data->n_vertexs);
    mat_mult_AdjList(file_data->graph, vec, new_vec, file_data->n_vertexs);
    new_norm_vec = normalize_vec(file_data->n_vertexs, new_vec);

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
    char timefilename[100] = "time_omp_";
    strcat(timefilename, input_filename);
    printf("%s\n", timefilename);
    time_record_file = fopen(timefilename, "a+");
    fprintf(time_record_file, "%.10lf\n", (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1e6);
    fclose(time_record_file);
    return 0;
}

void read_arguments_or_abort(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <number of elements> <number of arrays>\n",
                argv[0]);
        exit(505);
    }
}

double normalize_vec(int n_vertexs, double *vector)
{

    double sum_elements = 0;
#pragma omp parallel for default(none) shared(vector, n_vertexs) reduction(+ \
                                                                           : sum_elements) schedule(static)
    for (int i = 0; i < n_vertexs; i++)
    {
        sum_elements += pow(vector[i], 2);
    }

#pragma omp parallel for default(none) shared(vector, n_vertexs, sum_elements) schedule(static)
    for (int i = 0; i < n_vertexs; i++)
    {
        vector[i] /= sqrt(sum_elements);
    }
    return sqrt(sum_elements);
}

void mat_mult_AdjList(struct Graph *graph, double *vector, double *new_vector, int n_vertexs)
{

#pragma omp parallel for default(none) shared(new_vector, n_vertexs, vector, graph) schedule(dynamic)
    for (int i = 0; i < n_vertexs; i++)
    {
        struct Node *ptr = graph->head[i];
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

void cleanVector(double *Clean_vector, int n_vertexs)
{
    double zero = 0;
#pragma omp parallel for default(none) shared(Clean_vector, n_vertexs, zero) schedule(static)
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
#pragma omp parallel for default(none) shared(new_vector, vector, n_vertexs) schedule(static)
    for (int i = 0; i < n_vertexs; i++)
    {
        double aux = new_vector[i];
        vector[i] = aux;
    }
}