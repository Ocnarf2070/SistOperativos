#include <stdio.h>
int main()
{
  int i, pid, otropid,n=0;
  pid=fork();
  if (pid){ 
        pid=fork();
	n++;
	printf("%d\n",n);
	}
  else{
        otropid=fork();
	n++;
	printf("%d\n",n);
	}
  sleep(1);
}
