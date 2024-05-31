#include <linux/limits.h>
#include <stdio.h>
#include "LineParser.h"
#include <sched.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>


//ps - report a snapshot of the current processes.
//int kill(pid_t pid, int sig);
// The kill() system call can be used to send any signal to any process group or process.

       //If pid is positive, then signal sig is sent to the process with the ID specified by pid.

       //If pid equals 0, then sig is sent to every process in the process group of the calling process.

       //If  pid  equals  -1, then sig is sent to every process for which the calling process has permission to send signals,
       //except for process 1 (init), but see below.

void execute(cmdLine *pCmdLine) {
    if (strcmp(pCmdLine->arguments[0], "cd") == 0) { 
        if (chdir(pCmdLine->arguments[1]) == -1) { 
            fprintf(stderr, "cd: %s: No such file or directory\n", pCmdLine->arguments[1]); 
        }
        //copied from chat :::::: DELETEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
    // }
    //  else if (strcmp(pCmdLine->arguments[0], "alarm") == 0) { // Check if the command is "alarm"
    //     pid_t pid = atoi(pCmdLine->arguments[1]); // Get the process ID from the argument
    //     if (kill(pid, SIGCONT) == 0) { // Send SIGCONT signal to wake up the process
    //         printf("Process %d woke up\n", pid);
    //     } else {
    //         perror("kill() error"); // Print error message if kill fails
    //     }
    // } else if (strcmp(pCmdLine->arguments[0], "blast") == 0) { // Check if the command is "blast"
    //     pid_t pid = atoi(pCmdLine->arguments[1]); // Get the process ID from the argument
    //     if (kill(pid, SIGKILL) == 0) { // Send SIGKILL signal to terminate the process
    //         printf("Process %d terminated\n", pid);
    //     } else {
    //         perror("kill() error"); // Print error message if kill fails
    //     } 
    // }
    else {
        pid_t p = fork();
        if(p ==-1){
            perror("fork() error");
            exit(1);
        }
        if (p == 0) {
            fprintf(stderr, "PID: %d\n", getpid());
            fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);
            execvp(pCmdLine->arguments[0], pCmdLine->arguments);
            perror("execvp() error"); // If execvp returns, it means an error occurred, and the current process was not replaced. 
            _exit(1); //Used in situations where the process needs to terminate immediately without 
            //performing any cleanup, such as in the child process after a fork where performing such cleanup could be 
            //problematic or redundant.
        }
        else{
            if (pCmdLine->blocking) { // if blocking, wait for the child process to finish
                int status; //stores the exit status of the child process
                waitpid(p, &status, 0); // If blocking is false, the shell will not wait for the child process to finish.
                //The  waitpid()  system  call  suspends  execution  of the calling thread until a child specified by pid argument has
                //changed state.  By default, waitpid() waits only for terminated children, but this behavior is  modifiable  via  the
                //options argument, as described below.
            }
        }
    }

}

int main(){
    char cwd[PATH_MAX]; 
    char input[2048]; 

    while(1){
        if(getcwd(cwd, sizeof(cwd))!=NULL){
            printf("%s\n", cwd); 
            fflush(stdout);
        }
        else{
            perror("getcwd() error"); // get current working directory
            return 1; 
        }
        if(fgets(input, 2048, stdin)!=NULL){ // read input from the user
            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n') {
                input[len - 1] = '\0'; // remove newline character
            }
            if (strcmp(input, "quit") == 0) {
                    break;
            }
            cmdLine *parsedLine = parseCmdLines(input);
            if(parsedLine == NULL){
                perror("parseCmdLines() error");
                continue;
            }
            execute(parsedLine);
            freeCmdLines(parsedLine);
        }
        else{
            perror("fgets() error");
            printf("\n");
            return 1; 
        }    
    }
    return 0;
}