#include <stdio.h>
#include <stdlib.h>
#define THREADS 4 // numero de threads
#define SIZE 1000 // dimensin de las matrices cuadradas
pthread_t tid[THREADS]; //vector de threads creados
int thid[THREADS]; // numero de cada thread
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
void* multiplica(void *p) { // multiplica una parte de la matriz segn el identificador de thread
  int n,i,j,k,inicio,final;

  n=*(int*)p;
  inicio=(SIZE/THREADS)*n;
  if(n!=THREADS-1) final= (SIZE/THREADS)*(n+1)-1;
  else final=SIZE-1; //el ltimo se queda con el resto
  printf("|--- thread %d, multiplicando filas de %d a %d \n",n,inicio,final);

  for (j=inicio; j<=final; j++)
    for (i=0; i<SIZE; i++) {
      C[i][j] = 0.0;
      for (k=0; k<SIZE; k++) C[i][j] += A[i][k] * B[k][j];
    }
}
//----------------------------------------------------------
int main(int argc, char *argv[])
{
  int i, j, k;
  struct timeval start, end;
  rellena(); // inicializamos matrices
  printf("Calculando la multiplicación con un solo thread\n");
  gettimeofday(&start, NULL);
  for (j=0; j<SIZE; j++) // calculamos la multiplicación en serie
    for (i=0; i<SIZE; i++) {
      D[i][j] = 0.0;
      for (k=0; k<SIZE; k++) D[i][j] += A[i][k] * B[k][j];
    }
  gettimeofday(&end, NULL);
  printf("|--- tiempo transcurrido: %0.2f ms\n", ((end.tv_sec * 1000.0 + end.tv_usec/1000.0) - (start.tv_sec * 1000.0 + start.tv_usec/1000.0)));
  printf("Calculando la multiplicacin con %d threads\n",THREADS);
  gettimeofday(&start, NULL);
  for(i=0; i<THREADS; i++) thid[i]=i;
  for(i=0; i<THREADS; i++) pthread_create(&tid[i], NULL, multiplica, (void *) & thid[i]);
  for(i=0; i<THREADS; i++) pthread_join(tid[i],NULL); // esperamos a todos
  gettimeofday(&end, NULL);
  printf("|--- tiempo transcurrido: %0.2f ms\n", ((end.tv_sec * 1000.0 + end.tv_usec/1000.0) - (start.tv_sec * 1000.0 + start.tv_usec/1000.0)));
  printf("Comprobando coincidencia de los resultados\n");
  for (i=0; i<SIZE; i++)
    for (j=0; j<SIZE; j++) 
      if (C[i][j] != D[i][j])
        printf("|--- Valores distintos en [%d][%d]: %f - %f\n", i, j, C[i][j], D[i][j]);
}
