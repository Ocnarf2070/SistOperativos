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

#include <stdio.h>
#include "job_control.h"   // remember to compile with module job_control.c 

#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

// -----------------------------------------------------------------------
//                            MAIN          
// -----------------------------------------------------------------------
job * list;
job * tarea;
job * actual;

void listado(){
	if(empty_list(list)){
		printf("No hay ninguna tarea en segundo plano \n");
	} else {
		print_job_list(list);
	}
}

void manejador(){
	job *ac; 
	int st,inf; 
	enum status st_res; 
	int tam=list_size(list); 

	block_SIGCHLD();
	if(!empty_list(list)){
		while(tam!=0){		
			ac=get_item_bypos(list,tam); 
			if(waitpid(ac->pgid,&st,WNOHANG|WUNTRACED)>0){
				st_res=analyze_status(st,&inf);
				listado();
				if(st_res!=1){
					printf("\n Eliminado.");
					print_item(ac); 
					delete_job(list,ac);
				}
			}
			tam--; 
		}
	}
	unblock_SIGCHLD();
	//signal(SIGCHLD,manejador);
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

	ignore_terminal_signals();
	list = new_list("ListaProc");
	signal(SIGCHLD,manejador);
	
	while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
	{   
		
		printf("COMMAND->");
		fflush(stdout);
		
		get_command(inputBuffer, MAX_LINE, args, &background);  /* get next command */

		if(args[0]==NULL) continue;   // if empty command
		else if(strcmp(args[0], "jobs") == 0){		
			listado();
			continue;
		}else if(strcmp(args[0],"cd")==0){
			if(chdir(args[1])==-1)perror(args[1]);
			continue;
		}else if(strcmp(args[0],"bg")==0){
			int n=1;
			if(args[1]!=NULL)n=atoi(args[1]);
			if(empty_list(list)||n<=0||n>list_size(list)){
				printf("No existe ese trabajo\n");
				continue;
			}else{
				block_SIGCHLD();
				actual=get_item_bypos(list,n);
				if(strcmp(actual->command,"vi")==0||strcmp(actual->command,"nano")==0);
				else{
					actual->state=BACKGROUND;
					killpg(actual->pgid,SIGCONT);
				}
				printf("Background job running... pid: %d, command: %s\n",actual->pgid,actual->command);
	
				unblock_SIGCHLD();
			}
			
			continue;
			
		}else if(strcmp(args[0],"fg")==0){
			int n=1;
			if(args[1]!=NULL)n=atoi(args[1]);
			enum status st_res;
			if(empty_list(list)||n<=0||n>list_size(list)){	
				printf("No existe ese trabajo\n");
				continue;
			}else{
				block_SIGCHLD();
				actual=get_item_bypos(list,n);
				set_terminal(actual->pgid);
				if(actual->state==STOPPED)killpg(actual->pgid,SIGCONT);
				actual->state=FOREGROUND;
				waitpid(actual->pgid,&status,WUNTRACED);
				status_res=analyze_status(status,&info);
				printf("Foreground pid: %d, command: %s, %s, info: %d\n",actual->pgid,actual->command, status_strings[status_res], info);
				if(st_res==SUSPENDED)actual->state=STOPPED;
				else delete_job(list,actual);
				set_terminal(getpid());			
				unblock_SIGCHLD();
			}
			continue;
			
		}
		/* the steps are:
			 (1) fork a child process using fork()
			 (2) the child process will invoke execvp()
			 (3) if background == 0, the parent will wait, otherwise continue 
			 (4) Shell shows a status message for processed command 
			 (5) loop returns to get_commnad() function
		 */
		pid_fork=fork();
		
		

		if (pid_fork==0){
			restore_terminal_signals();
			pid_fork=getpid();
			new_process_group(pid_fork);
			if(background==0) set_terminal(pid_fork);
			execvp(args[0], args);
			printf("Error, command not found: %s\n",inputBuffer);
			exit(-1);
		}else{
			new_process_group(pid_fork);
			if(background==0)
			{
				set_terminal(pid_fork);
				pid_wait=waitpid(pid_fork, &status, WUNTRACED);
				status_res=analyze_status(status, &info);
				if(status_res==SUSPENDED){
					block_SIGCHLD();
					tarea = new_job(pid_fork,inputBuffer,STOPPED);
					add_job(list,tarea);
					unblock_SIGCHLD();
				}
				printf("Foreground pid: %d, command: %s, %s, info: %d\n",pid_wait,inputBuffer, status_strings[status_res], info);
				set_terminal(getpid());
			}else{
				tarea = new_job(pid_fork,inputBuffer,BACKGROUND);
				block_SIGCHLD();
				add_job(list,tarea);
				unblock_SIGCHLD();
				printf("Background job running... pid: %d, command: %s\n",pid_fork,inputBuffer);
				//fflush(stdout);
				//continue;
			}

		}

	} // end while
}
