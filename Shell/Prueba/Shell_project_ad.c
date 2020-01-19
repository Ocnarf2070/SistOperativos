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
#include "string.h" 

#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

job *lista; //Creamos un puntero a una lista a partir de job_control.h 
job *tarea; //creamos un proceso



//****Metodo para los procesos zombie****
void zombie(){
	job *ac; //--------------------------------Puntero a lista
	int st,inf; //-----------------------------variables estado e info
	enum status st_res; //---------------------variable de tipo de estado
	int tam=list_size(lista); //almaceno el tamaño de la lista
	
	block_SIGCHLD(); //bloque las señales
	if(empty_list(lista)!=1){ //---------------Esta funcion devolveria 1 si la lista está vacia, si no esta vacia pues la recorremos, que contendrá todos los procesos zombie
		while(tam!=0){			//---------------que hay y los mato con WNOHANG y lo elimino de la lista
			ac=get_item_bypos(lista,tam); //busca y devuelve un elemento de la lista, dada la lista y la posicion. Si no está devolvería NULL
			if(waitpid(ac->pgid,&st,WNOHANG)>0){ //wnohang hace que el proceso no se bloquee y devuelve el pid del proceso (si existe) o -1 si no existe
				st_res=analyze_status(st,&inf);
				if(st_res!=1){
					printf("\n Eliminado.");
					print_item(ac); //funcion en job_control.c que pinta toda la info sobre la tarea
					delete_job(lista,ac); //borramos la tarea de la lista
					listado();
				}
			}
			tam--; //cuando la lista llegue a un tam 0 ya terminamos
		}
	}
	unblock_SIGCHLD();	//-------------------Desbloqueo la señal
	signal(SIGCHLD,zombie);	//y envio la señal
}

void ayuda(){
	printf("Los comandos incorporados en el Shell son: \n \n");
	printf("jobs-> Devuelve el listado de las tareas en segundo plano o que estan suspendidas \n \n");
	printf("cd X-> Permite cambiar de directorio, el segundo argumento nos indica la direccion, si queremos volver atras ay que escribir \"cd ..\" \n \n");
	printf("fg X-> Pone una tarea que está en segundo plano o suspendida en primer plano.\n Si  \"fg\" lo pasamos sin argumento pone en primer plano el primero en la lista  \n \n");
	printf("bg X-> Pone una tarea que está suspendida, a ejecucion en segundo plano.\n El argumento es el numero dentro de la lista, si no le pasamos argumento coge el primero de la lista \n \n");
}

void primer_plano(){

}

void segundo_plano(){

}

void listado(){ //listamos los jobs
	if(empty_list(lista) == 1){
		printf("No hay ninguna tarea en segundo plano \n"); //si la lista esta vacia aparecera este mensaje
	} else {
		print_job_list(lista);	//pinta la lista
	}
}

// -----------------------------------------------------------------------
//                            MAIN          
// -----------------------------------------------------------------------

int main(void){

	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background;             /* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE/2];     /* command line (of 256) has max of 128 arguments */
	// probably useful variables:
	int pid_fork, pid_wait; /* pid for created and waited process */
	int status;             /* status returned by wait */
	enum status status_res; /* status processed by analyze_status() */
	int info;				/* info processed by analyze_status() */
	
	//********VARIABLES INCIALES	
	int i = 0;	//Contador que utilizaremos en varias ocasiones
	lista = new_list("lista");	
	ignore_terminal_signals();	

	while (1){  /* Program terminates normally inside get_command() after ^D is typed*/   		

		if(i == 0){
			i=1;
			printf("Has activado la señal SIGCHLD \n");
			printf("Para saber los comandos implementados escriba \"ayuda\" \n");
			signal(SIGCHLD, zombie); //SIGCHLD, cuando el hijo termina envia una señal al padre, por defecto se ignoraria y el proceso quedaria zombie pero nosotros hacemos que eso no ocurra
		}

		printf("adrian@Shell $~: ");
		fflush(stdout);
		get_command(inputBuffer, MAX_LINE, args, &background);  /* get next command */
		
		if(args[0]==NULL) continue;   // if empty command, va al final y comprueba si hay return, como no lo hay regresa al principio del bucle



		//Implementacion de comandos internos
		if(strcmp(args[0], "jobs") == 0){		
			listado();
			continue;
		}else if(strcmp(args[0], "cd") == 0){
			if(chdir(args[1]) == -1){
				printf("Error. El directorio \"%s\" no existe\n", args[1]);			
			}
			continue;
		}else if(strcmp(args[0], "fg") == 0){
			primer_plano();
			continue;
		}else if(strcmp(args[0], "bg") == 0){
			segundo_plano();
			continue;
		}else if(strcmp(args[0], "ayuda") == 0){
			ayuda();
			continue;		
		}
		//******************************	


		pid_fork = fork();
		
		if (pid_fork == 0){ //hijo
			restore_terminal_signals();
			pid_fork = getpid();	//obtenemos el pid 
			new_process_group(pid_fork); //creamos un nuevo grupo de procesos						
			if(background == 0 ) set_terminal(pid_fork); //sede al proceso el terminal 			
			execvp(args[0], args);		
			printf("Error. El comando \"%s\" no existe \n",args[0]);			
			exit(127); //Señal con la que nostros queremos que termine
			
		} else { //padre
			new_process_group(pid_fork);
			if(background == 0){ //quiere decir que el comando no va seguido de &
				
				
				set_terminal(pid_fork);	//Asignamos el terminal			
				pid_wait = waitpid(pid_fork, &status, WUNTRACED);
 				status_res = analyze_status(status, &info); // Analizamos el estado de terminación

				if(status_res == SUSPENDED){
					tarea = new_job(pid_fork, args[0], STOPPED);
					block_SIGCHLD();								
					add_job(lista,tarea);				
					unblock_SIGCHLD();				
				}
                		printf("Foreground pid: %d, command: %s, %s, info: %d \n", pid_wait, args[0],status_strings[status_res], info); //Mostramos el mensaje de estado del  	proceso				
				set_terminal(getpid());	//el shell recupera el terminal
			}else{ //que el comando va seguido de un &, por lo tanto se está ejecutando en BG ----BACKGROUND
				tarea = new_job(pid_fork, args[0], BACKGROUND); //new_job es una funcion de job_control.h
				
				block_SIGCHLD();				
				add_job(lista, tarea);
				
				unblock_SIGCHLD();
				printf("Background running... pid: %d. Ejecutando el comando '%s' en background\n", pid_fork, args[0]);
				continue;		
			}
					
		}

	} // end while
}
