#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
int varglobal;
void main(int argc, char *argv[]) {
  pid_t pidfork;
  varglobal = 1;
  pidfork = fork();
  if (pidfork == 0) { /* proceso hijo */
    printf("Hijo: valor de la variable antes de modificacin %d\n", varglobal);
    varglobal = 500;
    printf("Hijo: valor de la variable despues de modificacin %d\n", varglobal);
    sleep(1);
  } else {/* proceso padre */ 
    printf("Padre: valor de la variable antes de wait %d\n", varglobal);
    sleep(5);
    printf("Padre: valor de la variable despues de wait %d\n", varglobal);
  }
  exit(0); 
}
