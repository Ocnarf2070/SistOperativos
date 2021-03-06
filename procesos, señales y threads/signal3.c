#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
void manejador(int senal){
 printf("Se ha recibido una señal del proceso padre\n");
 sleep(2);
 exit(10);
}

void main(int argc, char *argv[]) {
  pid_t pidfork;
  int status;
  pidfork = fork(); // creamos proceso hijo
  if (pidfork == 0) { /* proceso hijo */
    int i=0;
    signal(SIGINT,manejador);
    printf("Hijo: pid %d: ejecutando...\n",getpid());
    while(1) {sleep(1); printf("Hijo: %d seg\n",++i); } // bucle infinito
  } else {/* proceso padre */ 
    sleep(5);
    printf("\nPadre: pid %d: mandando señal SIGINT\n",getpid());
    kill(pidfork,SIGINT);
    while (pidfork != wait(&status));
    if (WIFEXITED(status)) { // el proceso ha terminado con un exit()
      printf("El proceso termino con estado %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) { // el proceso ha terminado por la recepcion de una señal
      printf("El proceso termin al recibir la segnal %d\n", WTERMSIG(status));
    } else if (WIFSTOPPED(status)) { // el proceso se ha parado por la recepcion de una señal
      printf("El proceso se ha parado al recibir la segnal %d\n", WSTOPSIG(status));
    }
  }
  exit(0); 
}
