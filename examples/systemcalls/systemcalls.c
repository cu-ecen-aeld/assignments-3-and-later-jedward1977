#include "systemcalls.h"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
// #include <stdio.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{
    // printf("\n\n--------- do_system ------\n");
    // printf("CMD: %s", cmd);

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
	int ret;
    ret =  system(cmd);
	//printf("Status %d\n", ret);
    if (ret == 0) {
        return true;
    } else {
        return false;
    }
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    printf("\nExecuting do exec .......\n");
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
        //printf("\nCmd : %s, i : %d\n",command[i], i);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/

    int ret;
    int pid;
    int wstatus;
    

    if ((pid = fork()) == -1)
    {
        perror("Error opening new process:");
        va_end(args); 
        return false;
    }
    else if (pid == 0)
    {
        va_end(args);
        ret = execv(command[0], command);
        // printf("ret : %d", ret);
        if (ret == -1)
        {
            perror("Error on execv:");
            exit(1);
        }
        else
            exit(0);
    }
    else
    {
        if ((pid = waitpid(-1, &wstatus, 0)) == -1)
        {
            perror("Error on waitpid:");
            va_end(args);
            return false;
        }
        else
        {
            if (WIFEXITED(wstatus))
            {
                if (WEXITSTATUS(wstatus) == 1)
                {
                    printf("Error from waitpid: %d\n", WEXITSTATUS(wstatus));
                    return false;
                }
                else
                    printf("Successful return from waitpid: %d\n", WEXITSTATUS(wstatus));
            }
        }
    }
    va_end(args);
    return true;
}


/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    printf("\n\n---> Executing do_exec_redirect ....\n");
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a reference,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

// printf(" AQUI----> cmd[0]: %s, cmd[1]: %s, cmd[2]: %s\n", command[0], command[1], command[2]);

// First, create the file 
    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if (fd < 0){
        perror("open:");
        va_end(args);
        return false;
    }

    int pid;
    int status;
    int wstatus;
    

    if ((pid = fork()) == -1)
     {
         perror("Fork error:");
         va_end(args);
         close(fd);
         return false;
     }
     else if (pid == 0) //child process
     {
         va_end(args);
         if (dup2(fd, 1) < 0)
         {
            perror("dup2");
            close(fd);
            exit(1);
         }
         close(fd);
         if ((status = execv(command[0], command)) == -1)
         {
             perror("Error on execv:");
             exit(1);
         }
         else
             exit(0);
     }
     else //parent process
     {
         if ((pid = waitpid(-1, &wstatus, 0)) == -1)
         {
             perror("Error on waitpid:");
             va_end(args);
             close(fd);
             return false;
         }
         else
         {
             if (WIFEXITED(wstatus))
             {
                 if (WEXITSTATUS(wstatus) == 1)
                 {
                     printf("Error return from waitpid: %d\n", WEXITSTATUS(wstatus));
                     close(fd);
                     return false;
                 }
                 else
                     printf("Success return from waitpid: %d\n", WEXITSTATUS(wstatus));
             }
         }
     }
    va_end(args);
    close(fd);
    return true;
}
