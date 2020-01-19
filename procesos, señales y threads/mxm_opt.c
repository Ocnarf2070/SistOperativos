#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define SIZE 1000 // dimensin de las matrices cuadradas
double A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE], D[SIZE][SIZE]; // matrices a multiplicar y resultado
//----------------------------------------------------------
void rellena() { // inicializa las matrices con valores aleatorios
  int i,j;
  for (i=0; i<SIZE; i++) 
    for (j=0; j<SIZE; j++) {
      A[i][j] = (double)rand()/(double)RAND_MAX;
      B[i][j] = (double)rand()/(double)RAND_MAX;
    }
}
//----------------------------------------------------------
int main(int argc, char *argv[])
{
  int i, j, k;
  struct timeval start, end;
  rellena(); // inicializamos matrices
  printf("Calculando la multiplicacin con un solo thread\n");
  gettimeofday(&start, NULL);
  for (i=0; i<SIZE; i++) // calculamos la multiplicacin en serie
    for (j=0; j<SIZE; j++) {
      D[i][j] = 0.0;
      for (k=0; k<SIZE; k++) D[i][j] += A[i][k] * B[k][j];
    }
  gettimeofday(&end, NULL);
  printf("|--- tiempo transcurrido: %0.2f ms\n", ((end.tv_sec * 1000.0 + end.tv_usec/1000.0) - (start.tv_sec * 1000.0 + start.tv_usec/1000.0)));
  printf("Calculando la multiplicacin con en paralelo\n");
  gettimeofday(&start, NULL);
#pragma omp parallel for private(i,j,k) shared(A,B,C)
  for (i=0; i<SIZE; i++) // calculamos la multiplicacin en paralelo
    for (j=0; j<SIZE; j++) {
      C[i][j] = 0.0;
      for (k=0; k<SIZE; k++) C[i][j] += A[i][k] * B[k][j];
    }
  gettimeofday(&end, NULL);
  printf("|--- tiempo transcurrido: %0.2f ms\n", ((end.tv_sec * 1000.0 + end.tv_usec/1000.0) - (start.tv_sec * 1000.0 + start.tv_usec/1000.0)));
  printf("Comprobando coincidencia de los resultados\n");
  for (i=0; i<SIZE; i++)
    for (j=0; j<SIZE; j++) 
      if (C[i][j] != D[i][j])
        printf("|--- Valores distintos en [%d][%d]: %f - %f\n", i, j, C[i][j], D[i][j]);
}
