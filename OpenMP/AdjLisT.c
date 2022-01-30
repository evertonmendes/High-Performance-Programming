#include <stdio.h>
#include <stdlib.h>

struct Graph
{
    struct Node *head[1];
};

struct Edge
{
    int i, j;
    double weight;
};

struct Node
{
    int dest;
    double weight;
    struct Node *next;
};

struct Graph *createGraph(struct Edge edges[], int n, int n_vertexs)
{
    struct Graph *graph = (struct Graph *)malloc(sizeof(struct Graph) + sizeof(struct Node) * (n_vertexs - 1));

    for (int i = 0; i < n_vertexs; i++)
    {
        graph->head[i] = NULL;
    }

    for (int i = 0; i < n; i++)
    {
        int src = edges[i].i;
        int dest = edges[i].j;
        double weight = edges[i].weight;

        struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
        newNode->dest = dest;
        newNode->weight = weight;

        newNode->next = graph->head[src];

        graph->head[src] = newNode;
    }
    return graph;
}

void printGraph(struct Graph *graph, int n_vertexs)
{
    int i;
    for (i = 0; i < n_vertexs; i++)
    {
        struct Node *ptr = graph->head[i];
        while (ptr != NULL)
        {
            printf("%d —> %d ", i, ptr->dest);
            ptr = ptr->next;
        }

        printf("\n");
    }
}

struct File_data
{
    int n_vertexs;
    struct Graph *graph;
};

struct File_data *ReadPajek(char *filename)
{
    FILE *fp;
    fp = fopen(filename, "r");

    int n_vertexs, n_edges;
    int i_element, j_element;
    double weight;

    if (fscanf(fp, "%d", &n_vertexs))
    {
    }
    if (fscanf(fp, "%d ", &n_edges))
    {
    }

    struct Edge *Edges = malloc(2 * n_edges * sizeof(struct Edge));

    int count_equal_ij = 0;
    for (int edge = 0; edge < n_edges; edge++)
    {
        if (fscanf(fp, "%d %d %lf", &i_element, &j_element, &weight))
        {
        }

        if (i_element != j_element)
        {
            Edges[2 * edge - count_equal_ij].i = i_element;
            Edges[2 * edge - count_equal_ij].j = j_element;
            Edges[2 * edge - count_equal_ij].weight = weight;
            Edges[2 * edge + 1 - count_equal_ij].i = j_element;
            Edges[2 * edge + 1 - count_equal_ij].j = i_element;
            Edges[2 * edge + 1 - count_equal_ij].weight = weight;
        }
        else
        {
            Edges[2 * edge - count_equal_ij].i = i_element;
            Edges[2 * edge - count_equal_ij].j = j_element;
            Edges[2 * edge - count_equal_ij].weight = weight;
            count_equal_ij += 1;
        }
    }

    fclose(fp);

    struct Graph *graph = createGraph(Edges, 2 * n_edges - count_equal_ij, n_vertexs);

    struct File_data *file_data = malloc(sizeof(int) + sizeof(struct Graph) + sizeof(struct Node) * (n_vertexs - 1));
    file_data->n_vertexs = n_vertexs;
    file_data->graph = graph;

    return file_data;
}