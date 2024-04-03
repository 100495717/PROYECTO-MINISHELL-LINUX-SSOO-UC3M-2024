//P2-SSOO-23/24

//  MSH main file
// Write your msh source code here
//#include "parser.h"
#include <stddef.h>			/* NULL */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

#define MAX_COMMANDS 8

// files in case of redirection
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param)
{
	printf("****  Exiting MSH **** \n");
	//signal(SIGINT, siginthandler);
	exit(0);
}

/* myhistory */

/* myhistory */

struct command
{
  // Store the number of commands in argvv
  int num_commands;
  // Store the number of arguments of each command
  int *args;
  // Store the commands
  char ***argvv;
  // Store the I/O redirection
  char filev[3][64];
  // Store if the command is executed in background or foreground
  int in_background;
};

int history_size = 20;
struct command * history;
int head = 0;
int tail = 0;
int n_elem = 0;

void free_command(struct command *cmd)
{
    if((*cmd).argvv != NULL)
    {
        char **argv;
        for (; (*cmd).argvv && *(*cmd).argvv; (*cmd).argvv++)
        {
            for (argv = *(*cmd).argvv; argv && *argv; argv++)
            {
                if(*argv){
                    free(*argv);
                    *argv = NULL;
                }
            }
        }
    }
    free((*cmd).args);
}

void store_command(char ***argvv, char filev[3][64], int in_background, struct command* cmd)
{
    int num_commands = 0;
    while(argvv[num_commands] != NULL){
        num_commands++;
    }

    for(int f=0;f < 3; f++)
    {
        if(strcmp(filev[f], "0") != 0)
        {
            strcpy((*cmd).filev[f], filev[f]);
        }
        else{
            strcpy((*cmd).filev[f], "0");
        }
    }

    (*cmd).in_background = in_background;
    (*cmd).num_commands = num_commands-1;
    (*cmd).argvv = (char ***) calloc((num_commands) ,sizeof(char **));
    (*cmd).args = (int*) calloc(num_commands , sizeof(int));

    for( int i = 0; i < num_commands; i++)
    {
        int args= 0;
        while( argvv[i][args] != NULL ){
            args++;
        }
        (*cmd).args[i] = args;
        (*cmd).argvv[i] = (char **) calloc((args+1) ,sizeof(char *));
        int j;
        for (j=0; j<args; j++)
        {
            (*cmd).argvv[i][j] = (char *)calloc(strlen(argvv[i][j]),sizeof(char));
            strcpy((*cmd).argvv[i][j], argvv[i][j] );
        }
    }
}


/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char*** argvv, int num_command) {
	//reset first
	for(int j = 0; j < 8; j++)
		argv_execvp[j] = NULL;

	int i = 0;
	for ( i = 0; argvv[num_command][i] != NULL; i++)
		argv_execvp[i] = argvv[num_command][i];
}


/**
 * Main sheell  Loop
 */
int main(int argc, char* argv[])
{
	/**** Do not delete this code.****/
	int end = 0;
	int executed_cmd_lines = -1;
	char *cmd_line = NULL;
	char *cmd_lines[10];

	if (!isatty(STDIN_FILENO)) {
		cmd_line = (char*)malloc(100);
		while (scanf(" %[^\n]", cmd_line) != EOF){
			if(strlen(cmd_line) <= 0) return 0;
			cmd_lines[end] = (char*)malloc(strlen(cmd_line)+1);
			strcpy(cmd_lines[end], cmd_line);
			end++;
			fflush (stdin);
			fflush(stdout);
		}
	}

	/*********************************/

	char ***argvv = NULL;
	int num_commands;

	history = (struct command*) malloc(history_size *sizeof(struct command));
	int run_history = 0;

	while (1)
	{
		int status = 0;
		int command_counter = 0;
		int in_background = 0;
		signal(SIGINT, siginthandler);

		if (run_history)
    {
        run_history=0;
    }
    else{
        // Prompt
        write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));

        // Get command
        //********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
        executed_cmd_lines++;
        if( end != 0 && executed_cmd_lines < end) {
            command_counter = read_command_correction(&argvv, filev, &in_background, cmd_lines[executed_cmd_lines]);
        }
        else if( end != 0 && executed_cmd_lines == end)
            return 0;
        else
            command_counter = read_command(&argvv, filev, &in_background); //NORMAL MODE
    }
		//************************************************************************************************


		/************************ STUDENTS CODE ********************************/
	   if (command_counter > 0) {
           if (strcmp(argvv[0][0],"mycalc") == 0 && argvv[0][1] == NULL){
               printf("No se ha introducido el primer operando");
               return -1
           }
           if (strcmp(argvv[0][0],"mycalc") == 0 && argvv[0][2] == NULL){
               printf("No se ha introducido la operación");
               return -1
           }
           if (strcmp(argvv[0][0],"mycalc") == 0 && argvv[0][3] == NULL){
               printf("No se ha introducido el segundo operando");
               return -1
           }
			 else if (strcmp(argvv[0][0],"mycalc") == 0 && argvv[0][1] != NULL && argvv[0][2] != NULL && argvv[0][3] != NULL) {
                //Si los argumentos son correctos, coge el comando mycalc
                int arg1 = atoi(argvv[0][1]), arg2 = atoi(argvv[0][3]); // Convierte los argumentos en enteros
                char mensaje[100]; //Variable para guardar el mensaje
                if (strcmp(argvv[0][2], "add") == 0){
                    //Caso de la suma
                    char var_entorno[128];
                    char *p = var_entorno; //Puntero de la variable de entorno
                    if (p==NULL) {
                    //Inicializa el valor de Acc
                        p = "0";
                    }
                    //Guardamos en var_entorno el valor de Acc como string
                    sprintf(var_entorno,"%d",(atoi(var_entorno) + arg1 + arg2));
                    //Creamos la variable de entorno Acc con valor p
                    setenv("Acc",p,1);
                    //Guardamos en mensaje la salida
                    snprintf(mensaje,100,"[OK] %d + %d = %d; Acc %s\n",arg1,arg2,arg1+arg2,getenv("Acc"));
                }
                else if (strcmp(argvv[0][2], "mul") == 0 )
                {//Caso de que los argumentos esten multiplicados, guarda el mensaje en la variable mensaje
                    snprintf(mensaje, 100, "[OK] %d * %d - %d\n", arg1, arg2, arg1 * arg2);
                }
                else if (strcmp(argvv[0][2], "div") == 0 )
                {//Caso de que el argumento sea div
                    if (arg2 == 0)
                    {// Comprueba si se divide /0
                        snprintf(mensaje, 100, "[ERROR] No puedes dividir por 0\n");
                    }
                    else
                    {//Guarda en la variable mensaje, el mensaje
                        snprintf(mensaje, 100, "[OK] %d / %d = %d; Resto %d\n", arg1, arg2, arg1 / arg2, arg1 % arg2);
                    }
                }
                else
                { //Hay un error con la estructura del comando, de nuevo guardamos el mensaje en nuestra variable mensaje
                    snprintf(mensaje, 100, "[ERROR] La estructura del comando es mycalc <operando_1> <add/mul/div> <operando_2>\n");
                }
                if (mensaje[1] == '0')
                {
                    // Si mensaje[1] == 0 escribimos en el std_error
                    write(STERR_FILENO, mensaje, strlen(mensaje));
                }
                else
                {
                    // En cualquier otro caso escribimos en el std_output
                    write(STDOUT_FILENO, mensaje, strlen(mensaje));
                }
            }
            else if(strcmp(argvv[0][0],"myhistory")==0){
                if (argvv[0][1] == NULL){
                    //Se desea ver la lista de los últimos 20 comandos
                    int start = (head+1) % history_size;
                    int i = start;
                    int count = 1;
                    while(count<history_size){
                        if (history[i].num_commands == 0){
                            break;
                        }
                        //Imprimimos el número de comando actual
                        fprintf(stderr,"%d",count);
                        for (int j = 0; j < history[i].num_commands; j++){
                            for (int k = 0; k < history.[i].args[j]; k++){
                                fprintf(stderr,"%s ", history.[i].argvv[j][k]);
                            }
                            fprintf(stderr," | ");
                        }
                        fprintf(stderr,"\n");
                        i = (i+1) % history_size;
                        count++;
                    }
                    run_history = 1;
                }
                else{
                    int argumento = atoi(argvv[0][1]);
                    if (argumento >= 0 && argumento <= 19){
                        int indice = (head+1+argumento) % history_size;
                        if (history[indice].num_commands != 0){
                            fprintf(stderr,"Ejecutando el comando %d\n", argumento);
                            //Obtenemos el comando correspondiente
                            char *comando = history[argumento].argvv[0][0];
                            execvp(comando,history[indice].argvv[0]);
                            //

                        }
                    }

                    }
                }

           else
           {

               pid_t pid;
               int fd1, fd2, fd3; // ficheros para la entrada, la salida y errores

               // Creamos un array de tuberias del tamaño de los comandos - 1
               int fdpip[command_counter - 1][2];

               // Creamos las tuberias
               for (int x = 0; x < command_counter - 1; x++)
                {
                    if (pipe(fdpip[x]) == -1)
                    {
                        // Mostramos un mensaje de error si no se crea la tuberia
                        perror("No se puede crear la tuberia");
                    }
                }

               for (int x = 0; x < command_counter; x++)
               {
                   //Creamos al hijo
                   pid = fork();

                   if(pid == -1){
                       //Si hay algun error
                       perror("Hay un error al crear al hijo");
                       return(-1)
                   }
                   else if (pid == 0)
                   {
                       getCompleteCommand(argvv, x);
                       if (x == 0){
                           // En el primer comando
                           if(strcmp(filev[0], "0") != 0){
                               //Comprobamos que haya un fichero de entrada
                               if ((fd1 = open(filev[0], O_RDONLY)) < 0) {
                                   perror("No se puede leer el fichero de entrada");
                               }
                               //Cambiamos la entrada al fichero de entrada
                               dup2(fd1, STDIN_FILENO);
                               close(fd1);
                           }
                       }
                       if (command_counter == 1)
                       {
                           //Si solo tenemos un comando tenemos que mirar el fichero de salida y de error)
                           if (strcmp(filev[1], "0") != 0)
                           {
                               if ((fd2 = open (filev[1], O_WRONLY | O_CREAT | O_TRUNC, 0664)) < 0)
                               {
                                   perror("No se puede abrir el fichero de salida");
                               }
                               //Cambiamos la salida al fichero de salida
                               dup2(fd2, STDOUT_FILENO);
                               close(fd2);
                           }
                           if (strcmp(filev[2], "0") != 0)
                           {
                               if ((fd3 = open (filev[2], O_WRONLY | O_CREAT | O_APPEND, 0664)) < 0)
                               {
                                   perror("No se puede abrir el fichero de error");
                               }
                               //Cambiamos el error al fichero de error
                               dup2(fd3, STDERR_FILENO);
                               close(fd3);
                           }
                       }
                       else
                       {
                           //Cambiamos la salida a la primera tubería
                           dup2(fdpip[0][1], STDOUT_FILENO);
                       }
                   }
                   else if (i < command_counter - 1)
                   {
                       // Si el comando está entre el primero y el último
                       //Cambiamos la entrada del comando a la salida de la tuberia anterior
                       dup2(fdpip[x-1][0], STDIN_FILENO);
                       //Cambiamos la saida del comando a la entrada de la tuberia anterior
                       dup2(fdpip[x][1], STDOUT_FILENO);
                   }
                   else
                   { // Si el comando es el último de la secuencia, cambiamos el input del comando a la tubería anterior como output
                       dup2(fdpip[x - 1][0], STDIN_FILENO);
                       if (strcmp(filev[1], "0") != 0)
                       {// output file
                           if ((fd2 = open(filev[1], O_WRONLY | O_CREAT | O_TRUNC, 0664)) < 0)
                           {//Si no puede abrirse, error
                               perror("No se puede leer el fichero de salida");
                           }
                           dup2(fd2, STDOUT_FILENO);
                           close(fd2);
                       }
                       if (strcmp(filev[2], "0") != 0)
                       {//Fichero de errores
                           if ((fd3 = open(filev[2], O_WRONLY | O_CREAT | O_APPEND, 0664)) < 0)
                           {//Si no puede abrirse, error
                               perror("No se puede leer el fichero de salida");
                           }
                           //Cambiamos el standard error al fichero de errores
                           dup2(fd3, STDERR_FILENO);
                           close(fd3);
                       }
                   }
                   for (int y = 0; y < command_counter - 1; y++)
                   {//Cerramos todas las tuberías aunque ya lo estén
                       close(fdpip[y][0]); //Output
                       close(fdpip[y][1]); //Input
                   }
                   //Hacemos el execvp
                   execvp(argv_execvp[0], argv_execvp);
                   }
               else
               {//PADRE
                   if (x == 0 && command_counter > 1)
                   {//Para el primer comando solo cerramos el input de la primera tubería, no lo vamos a usar de nuevo
                       close(fdpip[0][1]);
                   }
                       if (x > 0)
                       {//cerramos todas las tuberías, no las vamos a volver a usar
                           close(fdp[x - 1][1]);
                           close(fdp[x - 1][0]);
                       }
                       if (in_background == 0)
                       {//Solo esperamos si background is False
                           wait(NULL);
                       }
                       else
                       {//No esperamos
                           if (x == command_counter - 1)
                           {//Imprime el pid del último comando, para mandatos simples sera también el último
                               printf("[%d]\n", pid);
                           }
                       }
                   }
               }
           }
		}
    }
    return 0
}
