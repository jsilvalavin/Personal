#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <mpi.h>
#include <cmath>
using namespace std;
#include <fstream>
ifstream file;


float* vector_generator(int n)
{
    float* vector = (float*) calloc(n, sizeof(float));
    
    for (int i=0; i<n; i++)
    {
        vector[i] = 1;
    }

    return vector;}
float** matrix_generator(int filas, int columnas) {
  float** matrix = (float**) calloc(filas, sizeof(int*));

  for (int i = 0; i < filas; i++) {
    matrix[i] = (float*) calloc(columnas, sizeof(int));
  }

  for (int i = 0; i < filas; i++) {
    for (int j = 0; j < columnas; j++) {
        matrix[i][j] = 0;
    }
  }
  return matrix;
}

void free_matrix(float** matrix, int filas) {
  for (int i = 0; i < filas; i++) {
    free(matrix[i]);
  }
  free(matrix);
}

void print_matrix(float** matrix, int filas, int columnas) {
  printf("\n");
  for (int i = 0; i < filas; i++) {
      for (int j = 0; j < columnas; j++) {
        printf("%f ", matrix[i][j]); 
      }
    printf("\n");
  }
}

void print_vectorf(float* vector, int n) {

  printf("\n");
  for (int i = 0; i < n; i++) {
      printf("%f ", vector[i]);
  }
  printf("\n");
}
void print_vectori(int* vector, int n) {

  printf("\n");
  for (int i = 0; i < n; i++) {
      printf("%i ", vector[i]);
  }
  printf("\n");
}


int main()
{
MPI_Init(NULL,NULL);
int world_size, world_rank;
MPI_Comm_size(MPI_COMM_WORLD, &world_size);
MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
if (world_rank == 0) {
    printf("Cantidad de procesos: %i\n\n", world_size);
}
printf("Proceso número: %i \n", world_rank);


file.open("matrix.txt");

float *my_matrix;
float tmp;
int n,ncols;
file >> n; // cantida de de filas de la matriz
file >> ncols;



int max_iterations = 1000000;

int localFilas = n / world_size;
int firstIndex = world_rank*localFilas;

if (world_rank == world_size-1) {
    localFilas += n % world_size;
} 
int my_firstrow = firstIndex;
int my_nrows = localFilas;



my_matrix = new float [my_nrows * ncols];
        for (int i=0; i<(my_firstrow)*ncols; i++) {
            file >> tmp;
        }
        for (int i=0; i<my_nrows*ncols; i++) {
            file >> my_matrix[i];
  
        }
  file.close();
// transform  my _matrix to actual matrix

float** loc_matrix = matrix_generator(localFilas,n);
int counter = 0;
for (int i =0;i<localFilas;++i){
    for (int j =0;j<n;++j){
        loc_matrix[i][j] = my_matrix[counter];
        counter += 1;
}}




// Hasta acá logramos darle los índices a cada proceso para la matriz ahora inicialicemos el b

float* locb = vector_generator(localFilas); // la parte del b que le corresponde a cada uno de estos 
// preparamos las cosas para la comunicación

int count[world_size];
int disp[world_size];


for (int i=0; i<=world_size-1;i++){
  count[i] = n / world_size;

  disp[i] = i*(n/world_size);
  
  if (i == world_size-1){count[i] += n % world_size;} 

}
float all_b[n];

// get all b vector
float* values;

int values_count = localFilas;

values =(float*)malloc(sizeof(float)* values_count);

float last_norm;
// aquí comenzamos las iteraciones
for(int z=0; z<max_iterations;++z){


for (int i = 0; i < localFilas; ++i)// sin esto no me funciona ni idea por qué perdon 
      {
          values[i] = locb[i]+0;
      }
MPI_Allgatherv(values, localFilas, MPI_FLOAT, all_b, count, disp, MPI_FLOAT, MPI_COMM_WORLD);
// calculate new b for each process

// iterate over rows
for (int i = 0; i<localFilas; ++i){
  locb[i] = 0;
  for(int j = 0; j<n; ++j){
  locb[i] += loc_matrix[i][j] * all_b[j];
  }
} 
// get norm of Ab_k (do this by mpi_allreduce and sending sum squares of local b )
float locsum = 0;
for (int i=0; i<localFilas;++i){
    locsum += pow(locb[i],2);
}
float normsum;
MPI_Allreduce(&locsum, &normsum, 1, MPI_FLOAT, MPI_SUM,MPI_COMM_WORLD);
float norm = pow(normsum,0.5);
if (z==max_iterations-1){last_norm= norm;}
// update locb using the norm

for (int i =0; i<localFilas;++i ){
    locb[i] = (float) locb[i] / (float) norm; // assert float operands
}

}/// finalize iterations 
// printf("Last norm: %f",last_norm);


// in order to get b_kAb_k we use for this code locb*norm  (last Ab_k calculated ) and values (Ab_{k-1} which means b_k) and then sum all these values in process 0 and 
// the same path use "values" to get b_k




float loc_bab = 0;
float loc_btb = 0;
for(int i =0; i<localFilas;++i){
loc_bab += values[i]*locb[i] * last_norm;
loc_btb += pow(values[i],2);
}
float all_bab,all_btb;
MPI_Reduce(&loc_bab,&all_bab,1, MPI_FLOAT, MPI_SUM, 0,MPI_COMM_WORLD);
MPI_Reduce(&loc_btb,&all_btb,1, MPI_FLOAT, MPI_SUM, 0,MPI_COMM_WORLD);
if (world_rank==0){

  printf("La aproximación del valor propio es: %f\n",all_bab/all_btb);
  printf("El error relativo de aproximación es: %f\n", abs(10-(all_bab/all_btb))/10);
  printf("Proceso terminado\n");
}


// free_matrix(loc_matrix,localFilas);
free(locb);
free(values);
delete[] my_matrix;
MPI_Finalize();

// free(all_b);
// free(disp);  // hacerle free a estas 3 cosas me tiraba errores nefastos 
// free(count);

    return 0;
}