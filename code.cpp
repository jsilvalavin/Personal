#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <mpi.h>
using namespace std;
#include <fstream>
ifstream file;

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

float* read_matrix(int nrows, int ncols, int firstrow, int localrows)
{
    float tmp;
    ifstream file;
    file.open("matrix.txt");

    float* out;
    out = new float [localrows * ncols];

    for (int i = 0; i < (firstrow - 1) * ncols; i++)
    {
        file >> tmp;
    }

    for (int i = 0; i < localrows * ncols; i++)
    {
        file >> out[i];
    }

    file.close();

    return out;
}

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

    // parametros
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

    // vector local
    float* localb = (float*)calloc(localrows, sizeof(float));
    // llenar vector
    for (int i = 0; i < ncols; i++)
    {
        localb[i] = 1;
    }
    print_vec(localb, localrows);

    // matriz local
    
    // valores de matriz
    // float* localvals = read_matrix(nrows, ncols, firstindex, localrows);
    
    // -----------------------------------------------
    MPI_Finalize();
    return 0;
}