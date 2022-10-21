#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <mpi.h>
using namespace std;
#include <fstream>
ifstream file;

double **matrix_generator(int filas, int columnas)
{
    double **matrix = (double **)calloc(filas, sizeof(double *));

    for (int i = 0; i < filas; i++)
    {
        matrix[i] = (double *)calloc(columnas, sizeof(double));
    }
    return matrix;
}

double* read_matrix(int nrows, int ncols, int firstrow, int localrows)
{
    double tmp;
    ifstream file;
    file.open("matrix.txt");

    double* out;
    out = new double [localrows * ncols];

    for (int i = 0; i < (firstrow - 1) * ncols; i++)
    {
        file >> tmp;
    }

    for (int i = 0; i < localrows * ncols; i++)
    {
        file >> my_matrix[i];
    }

    file.close();

    return out
}

void print_mat(double** matrix, int nrows, int ncols)
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

void print_vec(double* vec, int k)
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
    file.open("matrix.txt");
    double* matrix;
    double tmp;
    int nrows, ncols; // asumiendo matrices cuadradas!

    file >> nrows;
    file >> ncols;

    int iteraciones = 100000000;

    // Abrir mpi 
    MPI_Init(NULL, NULL);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

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
    double* localb = (double*)calloc(localrows, sizeof(double));
    // llenar vector
    for (int i = 0; i < ncols; i++)
    {
        localb[i] = 1;
    }
    print_vec(localb, localrows);

    // matriz local
    double** localmat = matrix_generator(localrows, ncols);
    
    // valores de matriz
    double* localvals = read_matrix(nrows, ncols, firstindex, localrows)
    
    // -----------------------------------------------
    MPI_Finalize();
    return 0;
}