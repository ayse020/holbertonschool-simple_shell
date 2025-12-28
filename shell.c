#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * _strcmp - Custom string comparison (for C89 compatibility)
 * @s1: First string
 * @s2: Second string
 *
 * Return: 0 if equal, difference otherwise
 */
int _strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

/**
 * print_env - Prints the current environment
 * @env: Environment variables array
 */
void print_env(char **env)
{
    int i = 0;
    
    while (env[i] != NULL)
    {
        printf("%s\n", env[i]);
        i++;
    }
}

/**
 * main - Simple Shell 1.0 with env built-in
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
    char *args[2];
    int interactive = isatty(STDIN_FILENO);
    
    (void)argc;
    
    while (1)
    {
        if (interactive)
        {
            write(STDOUT_FILENO, "$ ", 2);
        }
        
        read_chars = getline(&line, &len, stdin);
        
        if (read_chars == -1)
        {
            if (interactive)
                write(STDOUT_FILENO, "\n", 1);
            free(line);
            return (0);
        }
        
        if (line[read_chars - 1] == '\n')
            line[read_chars - 1] = '\0';
        
        if (strlen(line) == 0)
            continue;
        
        /* Check for built-in commands first */
        if (_strcmp(line, "env") == 0)
        {
            /* env built-in: print environment */
            print_env(env);
            continue;
        }
        else if (_strcmp(line, "exit") == 0)
        {
            /* exit built-in */
            free(line);
            return (0);
        }
        
        /* Prepare arguments for execve */
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
                exit(127);  /* Command not found exit code */
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
