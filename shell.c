#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * main - Simple Shell 0.1
 * @argc: Argument count (unused)
 * @argv: Argument vector
 * @env: Environment variables
 *
 * Return: Always 0
 */
int main(int argc, char **argv, char **env)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read_chars;
    pid_t child_pid;
    int status;
    char *args[2];  /* Fixed size array for C89 compatibility */
    
    (void)argc;
    
    while (1)
    {
        printf("#cisfun$ ");
        fflush(stdout);
        
        read_chars = getline(&line, &len, stdin);
        
        if (read_chars == -1)
        {
            printf("\n");
            free(line);
            return (0);
        }
        
        if (line[read_chars - 1] == '\n')
            line[read_chars - 1] = '\0';
        
        if (strlen(line) == 0)
            continue;
        
        /* Prepare arguments for execve - C89 compatible */
        args[0] = line;
        args[1] = NULL;
        
        child_pid = fork();
        if (child_pid == -1)
        {
            perror("fork");
            free(line);
            return (1);
        }
        
        if (child_pid == 0)
        {
            /* Child process */
            if (execve(args[0], args, env) == -1)
            {
                fprintf(stderr, "%s: No such file or directory\n", argv[0]);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            /* Parent process */
            wait(&status);
        }
    }
    
    free(line);
    return (0);
}
