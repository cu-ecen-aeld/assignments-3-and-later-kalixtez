#include "systemcalls.h"

#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
	int ret = system(cmd);
	if(ret == 127)
	{
		perror("Couldn't execute shell: ");
		return false;
	}

	if(cmd == NULL && ret == 0)
	{
		perror("No shell available: ");
		return false;
	}
	
	return true;
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
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
	int wstatus;
	pid_t fs = fork();

	if(fs == -1)
	{
		perror("fork error: ");
		return false;
	}
	
	if(fs != 0) // parent
	{	
		pid_t wr = waitpid(fs, &wstatus, 0); // wait for the child process to terminate execution.

		if(wr == -1)
		{
			perror("wait failed: ");
			return false;
		}
		
		if(WIFEXITED(wstatus))
		{
			printf("return value of child process == %d", WEXITSTATUS(wstatus));
			if(WEXITSTATUS(wstatus) == 0)
				return true;
			else
			{
				perror("Non-zero return value returned by child process: ");
				return false;
			}
		}

		else
		{
			perror("Child process could not be executed: ");
			return false;
		}	
	}
		
	else	// child
	{
		execv(command[0], command);
		
		// exec should never return if properly executed.
		perror("An error has occured: ");
		exit(4);
	}
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
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
	int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
	pid_t fs = fork();
	int wstatus;
	
	if(fs != 0) // parent
	{
		pid_t wr = waitpid(fs, &wstatus, 0); // wait for the child process to terminate execution.
		if(wr == fs)
			return true;

		else
		{
			perror("Child process could not be executed: ");
			return false;
		}	
	}
		
	else	// child
	{

		if (dup2(fd, 1) < 0)
		{ 
			perror("dup2: ");
			return false; 
		}

    	close(fd);
		execv(command[0], command);
		
		perror("An error has occured: ");
		exit(4);
	}
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

    va_end(args);
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

    va_end(args);

    return true;
}
