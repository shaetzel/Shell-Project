#include "command_line.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_LINE_LENGTH 512
void handleSigInt(int sig, siginfo_t * info, void * context){
    printf("CTRL + C Signal occurred\n");
}
void handleSigChild(int sig, siginfo_t * info, void * context){
    int status = 0;
    while(waitpid(-1,&status, WNOHANG) > 0)
    {   
        if(WIFEXITED(status)){
            printf("child exited with status %d\n", WEXITSTATUS(status));
        }
        else if(WIFSIGNALED(status)){
            printf("child exited due to signal %d\n", WTERMSIG(status));
        }
     }
}
int main(int argc, const char **argv)
{
    char cmdline[MAX_LINE_LENGTH];
    struct CommandLine command;
    struct sigaction action4SigInt;
    action4SigInt.sa_sigaction = handleSigInt;
    action4SigInt.sa_flags = SA_RESTART | SA_SIGINFO;
    sigemptyset(&action4SigInt.sa_mask);
    if(sigaction(SIGINT, &action4SigInt, NULL) < 0){
        perror("error establishing SIGINT handler");
        exit(1);
    }

    struct sigaction action4SigChild;
    action4SigChild.sa_sigaction = handleSigChild;
    action4SigChild.sa_flags = SA_RESTART | SA_SIGINFO;
    sigemptyset(&action4SigChild.sa_mask);
    if(sigaction(SIGCHLD, &action4SigChild, NULL) < 0){
        perror("error establishing SIGCHLD handler");
        exit(1);
    }
    for (;;)
    {

        printf("> ");
        fgets(cmdline, MAX_LINE_LENGTH, stdin);
        if (feof(stdin))
        {
            exit(0);
        }

        bool gotLine = parseLine(&command, cmdline);
        if (gotLine)
        {
            printCommand(&command);

       struct CommandLine * commandPtr = &command;
        if(strcmp(command.arguments[0], "cd") == 0){
            int cdOperation;
            if(commandPtr->arguments[1] == NULL){
                cdOperation = chdir(getenv("HOME"));
            }
            else{
                char *path = command.arguments[1];
                cdOperation = chdir(path);
            }
            if (cdOperation != 0){
                perror("Error changing directory");
            }

        }
       else if(strcmp(command.arguments[0], "exit") == 0){
        //use strcmp for all comparisons to strings   
           exit(0);
       }


       else{
           int pid = fork();

           if(pid == 0){
           //running as a child
               int execStatus = execvp(commandPtr->arguments[0],commandPtr->arguments);
               if(execStatus == -1){
                   perror("Error in execvp");
               }
           }
           else{
           //you're running as parent
               int status = 0;
               if(commandPtr->background == false){
                      waitpid(pid, &status, 0);
              }
          }
       }
        freeCommand(&command);
    }
 }
}

