/**
UNIX Shell Project

Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores - UMA

Some code adapted from "Fundamentos de Sistemas Operativos", Silberschatz et al.

To compile and run the program:
   $ gcc Shell_project.c job_control.c -o Shell
   $ ./Shell
	(then type ^D to exit program)

**/

#include "job_control.h"   // remember to compile with module job_control.c 

#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

// -----------------------------------------------------------------------
//                            MAIN
// -----------------------------------------------------------------------

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

    ignore_terminal_signals();  // TAREA1 el shell ignora las señales de terminal: ^C, ^Z, etc.

    while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
    {
        printf("andres@laboratorio $ ");
        fflush(stdout);
        get_command(inputBuffer, MAX_LINE, args, &background);  /* get next command */

        if(args[0]==NULL) continue;   // if empty command
		else if(strcmp(args[0],"cd")==0){
			if(chdir(args[1])==-1)perror(args[1]);
			continue;
		}
		
        pid_fork= fork();

        if(pid_fork==0)
        {   //hijo
            restore_terminal_signals(); // TAREA1 el hijo restaura sus señales de terminal: ^C, ^Z, etc.
            pid_fork=getpid();  // ojo que pid_fork era 0
            new_process_group(pid_fork); //TAREA1 crea nuevo grupo de procesos independiente
            if(background==0) set_terminal(pid_fork);  //TAREA1 asigna el terminal si es fg
            execvp(args[0], args);
            printf("Error, command not found: %s\n",inputBuffer);
            exit(-1);
        }
        else
        {
            // padre
            new_process_group(pid_fork);
            if(background==0)
            {   //fg
                set_terminal(pid_fork); //TAREA1 asigna el terminal si es fg
                pid_wait=waitpid(pid_fork, &status, WUNTRACED); //TAREA1 esperamos a que termine o suspenda
                status_res=analyze_status(status, &info); // TAREA1 analizamos terminación
                printf("Foreground pid: %d, command: %s, %s, info: %d\n",pid_wait,inputBuffer, status_strings[status_res], info); 
                set_terminal(getpid()); //TAREA1 el shell recupera el terminal
            }
            else
            {   //bg
                printf("Background job running... pid: %d, command: %s\n",pid_fork,inputBuffer);
                // TAREA1 no esperamos a que termine el proceso bg
            }

        } // end padre

    } // end while
}
