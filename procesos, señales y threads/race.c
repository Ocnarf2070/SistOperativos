#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#define IT 6
pthread_t tid[2];
long int ci, cd;
int N;
volatile long int i=0; /* eliminar optimizacin compilador*/
void* Incremento(void *p) {
  ci=0;
  while(i< N) 
    {if ((ci%100)==0) i= i + 1;
     ci++;
    }
  printf("|-- Hebra 1 (inc) llega a meta\n");
  pthread_exit(NULL);
}
void* Decremento(void *p) {
  cd=0;
  while (i> -N) 
    {if ((cd%100)==0) i= i-1;
     cd++;
    }
  printf("|-- Hebra 2 (dec) llega a meta\n"); 
  pthread_exit(NULL);
}
int main(void) {
  int j;
  N=10;
  for (j=0; j<IT; j++) {
    printf("Comenzando carrera con meta %d\n", N);
    i=0;
    pthread_create(&tid[0], NULL, Incremento, NULL);
    pthread_create(&tid[1], NULL, Decremento, NULL);
    pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);
    printf("|-- Iteraciones: en incremento: %lu, en decremento: %lu, diferencia: %lu\n",ci,cd,(ci>cd)?ci-cd:cd-ci); 
    N = N*10;
  }
}
