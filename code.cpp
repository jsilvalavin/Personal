#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <cmath>
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
    double start, end;

    // Abrir mpi
    MPI_Init(NULL, NULL);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // tiempo
    MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
    start = MPI_Wtime();

    file.open("matrix.txt");
    float* matrix;
    float tmp;
    int nrows, ncols; 

    file >> nrows;
    file >> ncols;

    int iteraciones = 100000;

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
    printf("Proceso %i \n", world_rank);

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
    for (int i = 0; i < world_size; i++)
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

    // vector resultante
    float* last_local_b;
    last_local_b = (float *)malloc(sizeof(float) * localrows); // probar coma

    // norma del vector y vector
    float b[nrows];
    float norm;

    // loop  -------------------------------------------
    for (int k = 0; k < iteraciones; k++)
    {
        for (int l = 0; l < localrows; l++)
        {
            last_local_b[l] = local_b[l];
        }
    

    // traer cosas
    MPI_Allgatherv(last_local_b, localrows, MPI_FLOAT, b, sizes, index, MPI_FLOAT, MPI_COMM_WORLD);

    // calcular nuevo b
    for (int i = 0; i < localrows; i++)
    {
        local_b[i] = 0; // resetear
        for (int j = 0; j < ncols; j++) // producto punto
        {
            local_b[i] += local_mat[i][j]*b[j];
        }
    }

    // calcular la suma de las componentes al cuadrado locales
    float local_sum = 0;
    for (int i = 0; i < localrows; i++)
    {
        local_sum += pow(local_b[i],2);
    }

    // obtener la norma entera
    float norm2 = 0;
    MPI_Allreduce(&local_sum, &norm2, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
    norm = pow(norm2, 0.5);

    for (int i = 0; i < localrows; i++) // normalizar por el vector
    {
        local_b[i] = (float) local_b[i] / (float) norm;
    }

    }

    // componentos locales de mu
    float local_numerator = 0;
    float local_denominator = 0;
    for (int i = 0; i < localrows; i++)
    {
        local_numerator += last_local_b[i]*(local_b[i]*norm);
        local_denominator += last_local_b[i]*last_local_b[i];
    }

    // encontrar mu
    float numerator, denominator;

    MPI_Reduce(&local_numerator, &numerator, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_denominator, &denominator, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);


    // Printeo final

    if (world_rank == 0)
    {
        printf("valor propio: %f\n", numerator/denominator);
        printf("Error relativo: %f \n", abs(10 - numerator/denominator)/10);

    }

    delete[] matrix;

    MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
    end = MPI_Wtime();


    MPI_Finalize();

    if (world_rank == 0)
    { /* use time on master node */
        printf("Runtime = %f\n", end - start);
    }
    return 0;
}