# Simple Shell Project

## Description
This is a simple UNIX command line interpreter (shell) implemented in C as part of the Holberton School curriculum. The shell supports basic command execution, built-in commands, and environmental variable management.

## Features
- Displays a prompt and waits for user input
- Executes commands with arguments
- Handles the PATH environment variable
- Implements built-in commands (exit, env, etc.)
- Supports command separators (;, &&, ||)
- Handles variables replacement ($?, $$)
- Implements comments (#)

## Installation
```bash
git clone https://github.com/your-username/holbertonschool-simple_shell.git
cd holbertonschool-simple_shell
gcc -Wall -Werror -Wextra -pedantic *.c -o hsh
