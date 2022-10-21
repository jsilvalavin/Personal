#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <mpi.h>
using namespace std;

double* read_matrix(int my_firstrow, int my_nrows)
{
    int nrows, ncols;
    double *my_matrix;
    double tmp;

    ifstream file;

    file.open("matrix.txt");

    if (file.is_open())
    {
        file >> nrows;
        cout << "Number of rows: " << nrows << endl;

        file >> ncols;
        cout << "Number of columns: " << ncols << endl;

        cout << "Read " << my_nrows << " rows starting from row " << my_firstrow << endl;

        my_matrix = new double[my_nrows * ncols];

        for (int i = 0; i < (my_firstrow - 1) * ncols; i++)
        {
            file >> tmp;
            cout << "skipped: " << tmp << endl;
        }

        cout << "Store matrix elements" << endl;
        for (int i = 0; i < my_nrows * ncols; i++)
        {
            file >> my_matrix[i];
            cout << i << " " << my_matrix[i] << endl;
        }

        file.close();
    }
    else
    {
        cout << "Unable to open file." << endl;
    }

    delete[] my_matrix;

    return my_matrix;
}

// double** make_matrix(double* matrix, int nrows, int ncols)
// {
//     double** out = new double [nrows * ncols];
//     for (int i = 0; i < nrows; i++)
//     {
//         for (int j = 0; j < ncols; j++)
//         {
//             out[i][j] = matrix[i*nrows + j];
//         }
//     }
//     return out;
// }

void print_matrix(float **matrix, int filas, int columnas)
{
    printf("\n");
    for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++)
        {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main(){
    // Setup --------------------------------------------------------------------------------------
    int ncols = 7;
    int n = ncols;
    int nrows = ncols;

    // Abrir mpi
    MPI_Init(NULL, NULL);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // numero procesos
    if (world_rank == 0)
    {
        printf("\nCantidad de procesos: %i\n", world_size);
        cout << "Numero de columnas: " << ncols << endl;
    }

    // Crear vector b0
    double b0[ncols];

    // Parametros locales
    int localrows, firstIndex;

    localrows = ncols / world_size;
    firstIndex = world_rank * localrows;
    
    if (world_rank == world_size - 1)
    {
        localrows += ncols % world_size;
    }

    // Sacar matriz local
    double* localmat_vec;
    localmat_vec = read_matrix(firstIndex, localrows);

    // double** localmat;
    // localmat = make_matrix(localmat_vec, localrows, ncols);

    print_matrix(localmat_vec, 1, localrows*ncols);

    // -----------------------------------------------
    MPI_Finalize();
    return 0;
}