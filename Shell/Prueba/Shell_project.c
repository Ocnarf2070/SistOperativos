/**
 UNIX Shell Project
 **/

#include "job_control.h"   // remember to compile with module job_control.c
#include <string.h>

#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

// -----------------------------------------------------------------------
//                            MAIN
// -----------------------------------------------------------------------
/*
  suspended -- stopped
  When a process is suspended with control z its status must be changed to Stopped
  Delete -- con las posiciones de la listaAux
  IF pid_wait == pgid
*/
job* list;

void manejador(){
    int status;
    int info;
    enum status status_res;
    int pid_wait;
    int perfNext = 0;

    job * aux;
    job * listaAux = list;
    printf("\n¡Signal SIGCHILD launched!\n");
    block_SIGCHLD();
    if(empty_list(listaAux)){
        printf("The process list is empty!\n");
    }else{
      listaAux = listaAux->next;
        while(listaAux != NULL){
          print_job_list(listaAux);
            pid_wait = waitpid(listaAux->pgid,&status, WNOHANG | WUNTRACED);
            if(pid_wait == (listaAux->pgid)){
              status_res = analyze_status(status, &info);
              if(status_res == SIGNALED || status_res == EXITED){
                aux = listaAux;
                printf("\nDeleting process pid: %d, Command: %s, %s, Info: %d\n",aux->pgid,aux->command,status_strings[status_res],info);
                perfNext = 1;
                listaAux = listaAux->next;
                delete_job(list,aux);
                free_job(aux);
              }
            }
          if(perfNext == 0) listaAux = listaAux->next;
          else perfNext = 0;



        }
    }
    unblock_SIGCHLD();
    fflush(stdout);
}


int main(void)
{
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE/2];     /* command line (of 256) has max of 128 arguments */
    // probably useful variables:
    int pid_fork, pid_wait; /* pid for created and waited process */
    int status;             /* status returned by wait */
    enum status status_res; /* status processed by analyze_status() */
    int info;				/* info processed by analyze_status() */


    list = new_list("ListaProc");
    signal(SIGCHLD,manejador);

    while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
    {
        ignore_terminal_signals();

        printf("COMMAND->");
        fflush(stdout);
        get_command(inputBuffer, MAX_LINE, args, &background);  /* get next command */

        if(args[0]==NULL){
           continue; // if empty command
        }else if(strcmp(args[0],"cd")== 0){
          //Separate if for "cd" as it has to check the directory beore executing the program
          if(chdir(args[1])==-1)
          perror(args[1]);
          continue;
        }else if(strcmp(args[0],"jobs")== 0){
          if(empty_list(list)){
            printf("The list is empty!\n");
          }else{
            block_SIGCHLD();
            print_job_list(list);
            unblock_SIGCHLD();
          }
        }else if(strcmp(args[0],"fg")== 0){
          if(empty_list(list)){
            printf("The list is empty!\n");
          }else{
            if(args[1] != NULL){
              int count; // gets args[1]
              job* listAux = list;
              listAux= listAux->next;

              while(count > 0){
                listAux= listAux->next;
                count--;
              }
              //Execute the process which is suspended and update the list

            }else{
              job* listAux = list;
              listAux= listAux->next;
              //Execute the program and update the list
            }
          }

        }else if(strcmp(args[0],"bg")== 0){
            //comm
        }else{
            pid_fork = fork();

            if(pid_fork == 0){//Chlild process
                restore_terminal_signals(); //restore terminal signals.
                pid_fork = getpid(); //pid_fork pierde el 0 y se le asocia el pid real del proceso creado.
                new_process_group(pid_fork); // crea un grupo de procesos independientes.
                if(background == 0){
                    set_terminal(pid_fork); //asignación del terminal si el proceso es fg.
                    execvp(args[0],args);
                    perror("Error, command not found");
                }
                exit(-1);
            }else{ //proceso padre.
                new_process_group(pid_fork);
                if(background == 0){
                    set_terminal(pid_fork);
                    pid_wait = waitpid(pid_fork, &status, WUNTRACED); //esperamos a la señal de terminación.
                    status_res = analyze_status(status, &info); //analizamos como ha terminado el proceso
                    if(status_res==SUSPENDED){
                      block_SIGCHLD();
                      job * item = new_job(pid_fork,args[0],STOPPED);
                      add_job(list,item);
                      unblock_SIGCHLD();
                    }
                    printf("\nForeground pid %d, command: %s, %s, info: %d\n", pid_wait, args[0], status_strings[status_res], info);
                    set_terminal(getpid()); //la shell recupera el terminal.
                }else{ //proceso en background
                    printf("\nBackground job running... pid: %d, command: %s\n", pid_fork,args[0]);
                    block_SIGCHLD();
                    job * item = new_job(pid_fork,args[0],background);
                    add_job(list,item);
                    unblock_SIGCHLD();
                }

            }
        }
    } // end while
}
