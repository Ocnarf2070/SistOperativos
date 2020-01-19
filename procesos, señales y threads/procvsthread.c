#include <stdio.h>
#include <stdlib.h>
#define NFORKS 100000
//----------------------------------------------------------
void *do_nothingT(void *null) { // funcin que ejecutaran los threads
  int i=0;
  pthread_exit(NULL);
}
//----------------------------------------------------------
void do_nothingP() { // funcin que ejecutarn los procesos
  int i=0;
  exit(0);
}
//----------------------------------------------------------
int main(int argc, char *argv[]) {
  int pid, j, status,rc;
  pthread_t tid;
  struct timeval start, end;
  double elapsed;
  printf("Creando %d procesos\n", NFORKS);
  gettimeofday(&start, NULL);
  for (j=0; j<NFORKS; j++) {
    if ((pid = fork()) < 0 ) { // comprobamos si se ha producido error
      printf ("fork failed with error code= %d\n", pid);
      exit(0);
    } else if (pid ==0) { // proceso hijo
      do_nothingP(); // hace el trabajo
    } else { // proceso padre
      waitpid(pid, status, 0); // espera la finalizacin del hijo
    }
  }
  gettimeofday(&end, NULL);
  elapsed = ((end.tv_sec * 1000.0 + end.tv_usec/1000.0) - (start.tv_sec * 1000.0 + start.tv_usec/1000.0));
  printf("|--- tiempo transcurrido: %0.2f ms (%0.2f ms por proceso de media)\n", elapsed, elapsed/NFORKS);
  printf("Creando %d threads\n", NFORKS);
  gettimeofday(&start, NULL);
  for (j=0; j<NFORKS; j++) {
    rc = pthread_create(&tid, NULL, do_nothingT, NULL); // creamos los threads y les asignamos el trabajo
    if (rc) { // comprobamos si se ha producido error 
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1); 
    }
    rc = pthread_join(tid, NULL); // esperamos finalizacin del thread
    if (rc) {
      printf("ERROR; return code from pthread_join() is %d\n", rc);
      exit(-1);
    }
  }
  gettimeofday(&end, NULL);
  elapsed = ((end.tv_sec * 1000.0 + end.tv_usec/1000.0) - (start.tv_sec * 1000.0 + start.tv_usec/1000.0));
  printf("|--- tiempo transcurrido: %0.2f ms (%0.2f ms por thread de media)\n", elapsed, elapsed/NFORKS);
}
