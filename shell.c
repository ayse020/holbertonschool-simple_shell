#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

extern char **environ;

/**
 * _strcmp - Custom string comparison
 * @s1: First string
 * @s2: Second string
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
 * _strdup - Custom strdup implementation
 * @str: String to duplicate
 * Return: Duplicated string or NULL
 */
char *_strdup(const char *str)
{
    char *dup;
    size_t len;
    
    if (str == NULL)
        return NULL;
    
    len = strlen(str) + 1;
    dup = malloc(len);
    if (dup == NULL)
        return NULL;
    
    memcpy(dup, str, len);
    return dup;
}

/**
 * find_command_in_path - Find command in PATH directories
 * @command: Command to find
 * Return: Full path if found, NULL otherwise
 */
char *find_command_in_path(char *command)
{
    struct stat st;
    char *path, *path_copy, *dir, *full_path;
    
    /* Check if command is absolute path */
    if (command[0] == '/')
    {
        if (stat(command, &st) == 0 && 
            (st.st_mode & S_IXUSR || st.st_mode & S_IXGRP || st.st_mode & S_IXOTH))
            return _strdup(command);
        return NULL;
    }
    
    /* Check if command is relative path */
    if (command[0] == '.' && command[1] == '/')
    {
        if (stat(command, &st) == 0 && 
            (st.st_mode & S_IXUSR || st.st_mode & S_IXGRP || st.st_mode & S_IXOTH))
            return _strdup(command);
        return NULL;
    }
    
    /* Get PATH environment variable */
    path = getenv("PATH");
    if (path == NULL || strlen(path) == 0)
    {
        /* If PATH is empty, use default paths */
        char *default_paths[] = {"/bin", "/usr/bin", NULL};
        int i;
        
        for (i = 0; default_paths[i] != NULL; i++)
        {
            full_path = malloc(strlen(default_paths[i]) + strlen(command) + 2);
            if (full_path == NULL)
                continue;
            
            sprintf(full_path, "%s/%s", default_paths[i], command);
            if (stat(full_path, &st) == 0 && 
                (st.st_mode & S_IXUSR || st.st_mode & S_IXGRP || st.st_mode & S_IXOTH))
                return full_path;
            
            free(full_path);
        }
        return NULL;
    }
    
    /* Search in PATH directories */
    path_copy = _strdup(path);
    if (path_copy == NULL)
        return NULL;
    
    dir = strtok(path_copy, ":");
    while (dir != NULL)
    {
        full_path = malloc(strlen(dir) + strlen(command) + 2);
        if (full_path == NULL)
        {
            free(path_copy);
            return NULL;
        }
        
        sprintf(full_path, "%s/%s", dir, command);
        
        if (stat(full_path, &st) == 0 && 
            (st.st_mode & S_IXUSR || st.st_mode & S_IXGRP || st.st_mode & S_IXOTH))
        {
            free(path_copy);
            return full_path;
        }
        
        free(full_path);
        dir = strtok(NULL, ":");
    }
    
    free(path_copy);
    return NULL;
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
 * main - Simple Shell 0.3 with PATH handling
 * @argc: Argument count (unused)
 * @argv: Argument vector
 * @env: Environment variables
 * Return: Exit status of last command
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
    int last_status = 0;
    char *command_path;
    
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
            return (last_status);
        }
        
        /* Remove trailing newline */
        if (read_chars > 0 && line[read_chars - 1] == '\n')
            line[read_chars - 1] = '\0';
        
        /* Skip empty lines */
        if (strlen(line) == 0)
            continue;
        
        /* Make a copy for strtok */
        line_copy = _strdup(line);
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
            last_status = 0;
            free(line_copy);
            continue;
        }
        else if (_strcmp(args[0], "exit") == 0)
        {
            free(line_copy);
            free(line);
            return (last_status);
        }
        
        /* Find command in PATH - Simple Shell 0.3 requirement */
        command_path = find_command_in_path(args[0]);
        
        if (command_path == NULL)
        {
            /* COMMAND NOT FOUND - DO NOT FORK! */
            fprintf(stderr, "%s: No such file or directory\n", args[0]);
            last_status = 127;
            free(line_copy);
            continue;
        }
        
        /* Execute external command - only fork if command exists */
        child_pid = fork();
        if (child_pid == -1)
        {
            perror("fork");
            free(command_path);
            free(line_copy);
            last_status = 1;
            continue;
        }
        
        if (child_pid == 0)
        {
            /* Child process */
            if (execve(command_path, args, env) == -1)
            {
                fprintf(stderr, "%s: No such file or directory\n", args[0]);
                free(command_path);
                free(line_copy);
                exit(127);
            }
        }
        else
        {
            /* Parent process - wait for child and get exit status */
            waitpid(child_pid, &status, 0);
            free(command_path);
            
            if (WIFEXITED(status))
            {
                /* Child exited normally */
                last_status = WEXITSTATUS(status);
            }
            else if (WIFSIGNALED(status))
            {
                /* Child terminated by signal */
                last_status = 128 + WTERMSIG(status);
            }
        }
        
        free(line_copy);
    }
    
    free(line);
    return (last_status);
}
