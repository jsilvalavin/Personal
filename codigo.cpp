#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <mpi.h>
using namespace std;

int main()
{
    // Leer matriz ---------------------------------------------------------------------------------
    int nrows, ncols;
    double *matrix;
    double tmp;

    ifstream file;

    file.open("matrix.txt");

    if (file.is_open())
    {
        file >> nrows;
        cout << "Number of rows: " << nrows << endl;

        file >> ncols;
        cout << "Number of columns: " << ncols << endl;

        int my_firstrow = 2;
        int my_nrows = 3;
        cout << "Read " << my_nrows << " rows starting from row " << my_firstrow << endl;

        matrix = new double[my_nrows * ncols];

        for (int i = 0; i < (my_firstrow - 1) * ncols; i++)
        {
            file >> tmp;
            cout << "skipped: " << tmp << endl;
        }

        cout << "Store matrix elements" << endl;
        for (int i = 0; i < my_nrows * ncols; i++)
        {
            file >> matrix[i];
            cout << i << " " << matrix[i] << endl;
        }

        file.close();
    }
    else
    {
        cout << "Unable to open file." << endl;
    }
    // Setup --------------------------------------------------------------------------------------

    // Abrir mpi
    MPI_Init(NULL, NULL);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    // numero procesos
    if (world_rank == 0)
    {
        printf("Cantidad de procesos: %i\n\n", world_size);
    }

    // Crear vector b0
    double b0[ncols]

        // Termino de código --------------------------------------------------------------------------
        delete[] matrix;

    return 0;
}