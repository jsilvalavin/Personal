#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <mpi.h>
using namespace std;
#include <fstream>
ifstream file;

void print_mat(float** matrix, int nrows, int ncols)
{
    printf("\n");
    for (int i = 0; i < nrows; i++)
    {
        for (int j = 0; j < ncols; j++)
        {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
}

void print_vec(float* vec, int k)
{
    printf("\n");
    for (int i = 0; i < k; i++)
    {
        printf("%f ", vec[i]);
    }
    printf("\n");
}

float **matrix_generator(int filas, int columnas)
{
    float **matrix = (float **)calloc(filas, sizeof(int *));

    for (int i = 0; i < filas; i++)
    {
        matrix[i] = (float *)calloc(columnas, sizeof(int));
    }

    for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++)
        {
            matrix[i][j] = 0;
        }
    }
    return matrix;
}

int main(){
    // Setup --------------------------------------------------------------------------------------
    // Abrir mpi
    MPI_Init(NULL, NULL);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    file.open("matrix.txt");
    float* matrix;
    float tmp;
    int nrows, ncols; 

    file >> nrows;
    file >> ncols;

    int iteraciones = 100000000;

    // mas parametros
    int firstindex, localrows;

    localrows = nrows / world_size;
    firstindex = world_rank * localrows;
    if (world_rank == world_size - 1)
    {
        localrows += nrows % world_size;
    }

    // numero procesos
    if (world_rank == 0)
    {
        printf("\nCantidad de procesos: %i\n", world_size);
    }

    // matriz de valores
    matrix = new float[localrows * ncols];
    for (int i = 0; i < (firstindex)*ncols; i++)
    {
        file >> tmp;
    }
    for (int i = 0; i < localrows * ncols; i++)
    {
        file >> matrix[i];
    }
    file.close();

    // matriz local
    float **local_mat = matrix_generator(localrows, ncols);

    // llenar matriz local
    int columna = 0;
    for (int i = 0; i < localrows; i++)
    {
        for (int j = 0; j < ncols; j++)
        {
            local_mat[i][j] = matrix[columna];
            columna += 1; 
            // esto cuenta 1 a 1 los elementos guardados
        }
    }

    // vector local
    float *local_b =(float *)calloc(localrows, sizeof(float));
    // llenar vector inicial
    for (int i = 0; i < localrows; i++)
    {
        local_b[i] = 1;
    }

    // pegar las cosas

    // tamaños de envio
    int sizes[world_size];
    for (int i = 0; i < world_size, i++)
    {
        sizes[i] = nrows/world_size;
        if (i == world_size - 1)
        {
            sizes[i] += nrows % world_size;
        }
    }
    
    // indices de donde van
    int index[world_size];
    for  (int i = 0; i < world_size; i++)
    {
        index[i] = i*(nrows / world_size);
    }

    // juntar vector b


    free(local_b);
    //free(values);
    //delete[] my_matrix;
    MPI_Finalize();
    return 0;
}