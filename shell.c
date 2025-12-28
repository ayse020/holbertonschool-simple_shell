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
 * parse_arguments - Splits a line into arguments
 * @line: Input line
 * @args: Array to store arguments
 *
 * Return: Number of arguments
 */
int parse_arguments(char *line, char *args[])
{
    int i = 0;
    char *token;
    
    token = strtok(line, " \t\n");
    while (token != NULL && i < 63)
    {
        args[i] = token;
        i++;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
    return i;
}

/**
 * main - Simple Shell 0.4 with env built-in
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
    char *args[64];
    int interactive = isatty(STDIN_FILENO);
    char *line_copy;
    int arg_count;
    
    (void)argc;
    (void)argv;
    
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
        
        /* Remove trailing newline */
        if (read_chars > 0 && line[read_chars - 1] == '\n')
            line[read_chars - 1] = '\0';
        
        /* Skip empty lines */
        if (strlen(line) == 0)
            continue;
        
        /* Make a copy for strtok (strtok modifies the string) */
        line_copy = strdup(line);
        if (line_copy == NULL)
        {
            perror("strdup");
            continue;
        }
        
        /* Parse arguments */
        arg_count = parse_arguments(line_copy, args);
        
        if (arg_count == 0)
        {
            free(line_copy);
            continue;
        }
        
        /* Check for built-in commands */
        if (_strcmp(args[0], "env") == 0)
        {
            print_env(env);
            free(line_copy);
            continue;
        }
        else if (_strcmp(args[0], "exit") == 0)
        {
            /* exit built-in - no arguments for now */
            free(line_copy);
            free(line);
            return (0);
        }
        
        /* Execute external command */
        child_pid = fork();
        if (child_pid == -1)
        {
            perror("fork");
            free(line_copy);
            continue;
        }
        
        if (child_pid == 0)
        {
            /* Child process */
            if (execve(args[0], args, env) == -1)
            {
                /* Show error from the command, not shell */
                perror(args[0]);
                free(line_copy);
                exit(127);
            }
        }
        else
        {
            /* Parent process */
            wait(&status);
        }
        
        free(line_copy);
    }
    
    free(line);
    return (0);
}
