#define _POSIX_SOURCE

#include "commands.h"
#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "errors.h"
#include <sys/wait.h> 
#include <stdlib.h>
#include <string.h>

void commandStart(char *argv[], int argc) {
    char *args[argc];
    pid_t commandId;

    for (int i = 0; i < argc; i++) {
        args[i] = argv[i];
    }
    args[argc] = NULL;

    if ((commandId = fork()) == 0) {
        if (execvp(args[0], args) == -1) {
            switch (errno) {
                case EACCES:
                    errorPermission();
                    break;
                case ENOMEM:
                    errorMemory();
                    break;
                case ENOENT:
                    errorNotSuchExecutable();
                    break;
            }
        }
    } else {
        printf("myshell: processo %d iniciado.\n", commandId);
        wait(0);
    }
}


void commandWait() {
    pid_t pid;

    int corpse;
    int status;
    int childrenCount = 0;

    while ((corpse = waitpid(0, &status, 0)) > 0) {
        childrenCount++;
        //while (wait(&status) != pid);
        if (status == 0) printf("myshell: processo %d finalizou normalmente com status %d.\n", corpse, status);
        else {
            char * signalDescription;
            if (status == 1) signalDescription = "SIGHUP";
            else if (status == 2) signalDescription = "SIGINT";
            else if (status == 3) signalDescription = "SIGQUIT";
            else if (status == 4) signalDescription = "SIGILL";
            else if (status == 5) signalDescription = "SIGTRAP";
            else if (status == 6) signalDescription = "SIGABRT";
            else if (status == 7) signalDescription = "SIGEMT";
            else if (status == 8) signalDescription = "SIGFPE";
            else if (status == 9) signalDescription = "SIGKILL";

            printf("myshell: processo %d finalizou de forma anormal com sinal %d: %s.\n", corpse, status, signalDescription);
        }
    }
    if (childrenCount == 0) printf("myshell: não há processos restantes.\n");
}

void commandWaitFor(char *argv[]){
    pid_t process;
    //int process = argv[1];
    int status;
    int childrenCount = 0;

    if (kill(process, 0) == -1) {
        printf("processo inexistente ou sem permissão!\n");
        return;
    }

    while ((waitpid(process, &status, 0)) > 0) {
        childrenCount++;
        if (status == 0) printf("myshell: processo %d finalizou normalmente com status %d.\n", process, status);
        else {
            char * signalDescription;
            if (status == 1) signalDescription = "SIGHUP";
            else if (status == 2) signalDescription = "SIGINT";
            else if (status == 3) signalDescription = "SIGQUIT";
            else if (status == 4) signalDescription = "SIGILL";
            else if (status == 5) signalDescription = "SIGTRAP";
            else if (status == 6) signalDescription = "SIGABRT";
            else if (status == 7) signalDescription = "SIGEMT";
            else if (status == 8) signalDescription = "SIGFPE";
            else if (status == 9) signalDescription = "SIGKILL";

            printf("myshell: processo %d finalizou de forma anormal com sinal %d: %s..\n", process, status, signalDescription);
        }
    }
    if (childrenCount == 0) printf("myshell: não há processos restantes.\n");
}

void commandChdir(char *path) {
    int cd = chdir(path);
    if(cd<0){
        printf("myshell: erro ao mudar diretorio\n");
    }
}

void commandPwd(){
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == NULL){
        perror("myshell: getcwd() error\n");
    } else{
        printf("%s\n", cwd);
    }
}

void proc_exit(int signal) {
    printf("%d::%dfui executado\n", signal, errno);
}

void commandWatchdog(char *argv[], int argc) {
    int sleepTimeSeconds = atoi(argv[0]);
    pid_t commandId;

    char *args[argc - 1];

    for (int i = 0; i < argc; i++) {
        args[i] = argv[i + 1];
    }
    args[argc - 1] = NULL;

    if ((commandId = fork()) == 0) {
        if (execvp(args[0], args) == -1) {
            switch (errno) {
                case EACCES:
                    errorPermission();
                    break;
                case ENOMEM:
                    errorMemory();
            }
        }
    } else {
        printf("erno1:%d\n", errno);
        signal (SIGCHLD, proc_exit);
        if (sleep(sleepTimeSeconds) == 0) {
            printf("myshell: finalizando processo 70, pois excedeu o tempo limite\n");
            printf("vou matar\n");
            kill(commandId, SIGKILL);
            printf("matei%d\n", errno);
        } else {

        }
    }

    wait(0);

}

void commandRun(char *argv[], int argc){
    /* Combina comportamento do start e wait
        Deve iniciar um programa, 
        esperar que tal processo termine
        imprimir status do termino */

    commandStart(argv[1], argc);
    commandWait();

}
