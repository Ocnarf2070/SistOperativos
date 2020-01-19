#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
int var_global = 0;
void *do_work(void *null) {  // funcin que ejecutar el nuevo thread
  int i = 500;
  sleep(5);
  var_global = i;
  pthread_exit(NULL);
}
int main(int argc, char *argv[]) {
  int rc;
  pthread_t tid;
  printf("Valor de la variable global antes de crear el thread: %d\n", var_global);
  rc = pthread_create(&tid, NULL, do_work, NULL);
  if (rc) { 
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
  }
  /* Wait for the thread */
  rc = pthread_join(tid, NULL);
  if (rc) {
    printf("ERROR; return code from pthread_join() is %d\n", rc);
    exit(-1);
  }
  printf("Valor de la variable global despues de ejecutar el thread: %d\n", var_global);
}
