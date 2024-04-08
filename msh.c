//P2-SSOO-23/24
//  MSH main file
// Write your msh source code here
//#include "parser.h
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

#define MAX_COMMANDS 8


// ficheros por si hay redirección
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param)
{
    printf("****  Saliendo del MSH **** \n");
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
 * Main shell  Loop
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
    int num_commands, status;
    history = (struct command*) malloc(history_size *sizeof(struct command));
    int run_history = 0;
    //seteamos la variable de entorno Acc
    setenv("Acc","0",1);
    int contador = 0;
    while (1)
    {
        int status = 0;
        int command_counter = 0;
        int in_background = 0;
        signal(SIGINT, siginthandler);

        if (run_history)
        {
            run_history=0;
        } else{
            write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));
        }

        // Get command
        //********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
        executed_cmd_lines++;
        if( end != 0 && executed_cmd_lines < end) {
            command_counter = read_command_correction(&argvv, filev, &in_background, cmd_lines[executed_cmd_lines]);
        }else if( end != 0 && executed_cmd_lines == end)
            return 0;
        else
            command_counter = read_command(&argvv, filev, &in_background); //NORMAL MODE
        //************************************************************************************************


        /************************ STUDENTS CODE ********************************/
        if (command_counter > 0) {
            if (command_counter > MAX_COMMANDS)
                printf("Error: Numero máximo de comandos es %d \n", MAX_COMMANDS);
            else
            {	//guardamos los descriptores 
                int desc_in = dup(0);
                int desc_out = dup(1);
                int desc_error = dup(2);

                if(strcmp(argvv[0][0],"mycalc")==0)
                    {
                    //variables que vamos a utilizar
                    char mensaje[100], mensajito[100];
                    int bien = 0;
                    int suma;
                    int operando_1 = atoi(argvv[0][1]);
                    int operando_2 = atoi(argvv[0][3]);
                    if(strcmp(argvv[0][2],"add")==0)
                    {
                        suma = operando_1 + operando_2;

                   
                        snprintf(mensajito,"%i",suma + atoi(getenv("Acc")));

                        //establecemos el nuevo Acc en mensajito
                        if (setenv("Acc",mensajito,1) < 0){

                            snprintf(mensaje, "Error al dar valor a la variable de entorno\n");
                            
                        }
                        snprintf(mensaje,"[OK] %i + %i = %i; Acc %s\n",operando_1,operando_2,suma,getenv("Acc"));
			bien = 1
                        
                        if(bien == 1){

                            write(STDERR_FILENO, mensaje, strlen(mensaje));
                            continue;
                        }
                        else {
                            write(STDOUT_FILENO, mensaje, strlen(mensaje));
                            continue;
                        }    

                    }
                    else if(strcmp(argvv[0][2], "mul")==0){
                        fprintf(stderr, "[OK] %d * %d = %d\n", operando_1, operando_2, operando_1 * operando_2);

                        
                            continue;
                        }
                    }

                    else if(strcmp(argvv[0][2],"div")==0)
                    {//comprobamos que no se divide entre cero
                        if (operando_2 != 0)
                        {
                            fprintf(stderr, "[OK] %d / %d = %d; Resto %d \n", operando_1, operando_2,  (operando_1 / operando_2), (operando_1 % operando_2));

                           
                                continue;
                            }

                        }
                        else
                        {
                            fprintf(stdout, "[ERROR] No se puede dividir entre cero\n")
                                continue;
                            
                        }
                    }
               	    else {
               	    	fprintf(stdout, "[ERROR] La estructura del comando es mycalc <operando_1> <add/mul/div> <operando_2>\n");
               	    	continue;
                }
                else if(strcmp(argvv[0][0],"myhistory")==0)
                {
                    int i;

                    if (argvv[0][1] != NULL) {
                        // Mostrar un comando específico del historial
                        int index = atoi(argvv[0][1]) - 1;
                        int history_size = 20;
                        if (index + 1 < 0 ||  index  >= history_size){
                            printf("ERROR: Comando no encontrado\n");
                            continue;
                        }
                        else{
                            fprintf(stderr,"Ejecutando el comando %d\n", index + 1);
                            struct command *cmd = &history[index];
        for (int i = 0; i < cmd->num_commands; i++) {
            getCompleteCommand(cmd->argvv, i);
            execvp(argv_execvp[0], argv_execvp);


                        }
                        }

                    } else {
                        // Mostrar el historial completo
                        //No sabemos cómo acceder a los comandos
                        for (int i = 0; i < contador; i++) {

                            fprintf(stderr, "%d ", i );
                            struct command *cmd = &history[i];
                            for (int j= 0; j < cmd->num_commands; j++){
                                for (int k = 0; k<cmd->args[j];k++){
                                    fprintf(stderr, "%s ", cmd->argvv[j][k]);
                                }
                                fprintf(stderr,"| ");
                            }
                            fprintf(stderr, "\n");


                        }
                        continue;

                    }
                }

                else
                {//procesos multiples con redirecciones y pipes
                    int p_desc[2];
                    int pipe_ok, p10, desc;
                    pid_t pid;
                    //vamos a hacer las redirecciones en el padre ya que luego las van a heredar los hijos directamente
                    //para asegurar que cada hijo reciba la que le corresponda crearemos redirecciones condicionales mas adelante
                    if(strcmp(filev[0],"0")!=0)
                    {
                        if ((desc = open(filev[0],O_RDONLY))<0){
                            fprintf(stdout, "[ERROR] Error al abrir desc_in\n");
                            continue;
                        }
                        if (dup2(desc,0)<0){
                            fprintf(stdout, "[ERROR] Error dup2 desc_in\n");
                            if (close(desc)<0){
                                fprintf("[ERROR] Error al cerrar desc_out\n");
                            }
                            continue;
                        }
                        if (close(desc)<0){
                            fprintf("[ERROR] Error cerrando desc_in\n");
                            continue;
                        }
                    }
                    if(strcmp(filev[1],"0")!=0)
                    {
                        if ((desc = open(filev[1],O_TRUNC | O_CREAT| O_WRONLY, 0644))<0){
                            fprintf(stdout, "[ERROR] Error al abrir desc_out\n");
                            continue;
                        }
                        if (dup2(desc,1)<0){
                            fprintf(stdout, "[ERROR] Error dup2 desc_out\n");
                            if (close(desc)<0){
                                fprintf(stdout, "[ERROR] Error cerrando desc_out\n");
                            }
                            continue;
                        }
                        if (close(desc)<0){
                            fprintf(stdout, "[ERROR] Error cerrando desc_out\n");
                            continue;
                        }
                    }
                    if(strcmp(filev[2],"0")!=0)
                    {
                        if ((desc = open(filev[2],O_TRUNC | O_CREAT| O_WRONLY, 0644))<0){
                            fprintf(stdout, "[ERROR] Error al abrir desc_error\n");
                            continue;
                        }
                        if (dup2(desc,2)<0){
                            fprintf(stdout, "[ERROR] Error dup2 desc_error\n");
                            if (close(desc)<0){
                                fprintf(stdout, "[ERROR] Error al cerrar desc_out\n");
                            }
                            continue;
                        }
                        if (close(desc)<0){
                            fprintf(stdout, "[ERROR] Error cerrando desc_error\n");
                            continue;
                        }
                    }

                    for (int i=0; i<command_counter; i++)
                    {
                        //mientras que no estemos en el ultimo proceso, crearemos una pipe nueva por iteración
                        if (i != (command_counter-1)){
                            if ((pipe_ok = pipe(p_desc))<0){
                                fprintf(stdout, "[ERROR] Error creando pipe");
                                continue;
                            }
                        }
                        //2. Creamos el fork
                        pid = fork();
                        if (pid<0)
                        {
                            fprintf(stderr, "fork: ");
                            if((close(p_desc[0])) < 0 && command_counter>1){
                                fprintf(stdout, "[ERROR] Error al cerrar descriptor");
                                continue;
                            }
                            if((close(p_desc[1])) <0 && command_counter>1){
                                fprintf(stdout, "[ERROR] Error al cerrar descriptor");
                                continue;
                            }
                            return (-1); //################################################################
                            //cerramos el proceso entero, padre e hijo
                        }

                        //3. redirecciones y 4. limpieza
                        if (0 == pid){
                            if (i!=0){
                                //redireccion de entrada
                                if (dup2(p10,0)<0){
                                    fprintf(stdout, "[ERROR] Error dup2 en redireccion de entrada\n");
                                    exit(-1);//#################################################
                                }
                                if (close(p10)<0){
                                    fprintf(stdout, "[ERROR] Error al cerrar pipe\n");
                                    exit(-1);//#################################################
                                }

                            }
                            if (i!=command_counter-1){
                                //redireccion de salida
                                if (dup2(p_desc[1],1)<0){
                                    fprintf(stdout, "[ERROR] Error dup2 en redireccion de salida\n");
                                    exit(-1);//#################################################
                                }
                                //limpieza en todas las pipes menos para el ultimo proceso
                                if (close(p_desc[0])<0){
                                    fprintf(stdout, "[ERROR] Error al cerrar pipe\n");
                                    exit(-1);//#################################################
                                }
                                if (close(p_desc[1])<0){
                                    fprintf(stdout, "[ERROR] Error al cerrar pipe\n");
                                    exit(-1);//#################################################
                                }
                            }

                            //ejecutamos proceso en hijo
                            execvp(argvv[i][0], argvv[i]);

                            //si pasamos de esta linea es xq algo ha ido mal como vimos en clase
                            fprintf(stdout, "[ERROR] Error al ejecutar\n");
                            exit(-1);


                            //final proceso hijo
                        }
                        else{
                            if (i!=command_counter-1){
                                if (close(p_desc[1])<0){
                                    fprintf(stdout, "[ERROR] Error al cerrar pipe\n");
                                    exit(-1);//#################################################
                                }
                                p10 = p_desc[0];
                            }
                            else if(command_counter>1){
                                //solo se ejecuta si se han creado pipes
                                if (close(p_desc[0])<0){
                                    fprintf(stdout, "[ERROR] Error al cerrar el pipe\n");
                                    exit(-1);//#################################################
                                }
                            }
                            if(in_background == 0){
                                while (pid != wait(&status));
                                if (status <0){
                                    fprintf(stdout, "[ERROR] Error en proceso hijo");
                                }
                            }
                            else{
                                printf("[%d]\n",pid);
                            }
                            //final proceso padre
                        }

                        //final del bucle for
                    }
                    dup2(desc_in,0);
                    dup2(desc_out,1);
                    dup2(desc_error,2);
                    close(desc_in);
                    close(desc_out);
                    close(desc_error);
                    //final del else
                }
            }

        }
        contador++;
    }
    return 0;
}
