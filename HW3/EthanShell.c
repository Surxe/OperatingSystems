#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ncurses.h>

#define MAX_ARGS 64
#define MAX_CMD_LENGTH 1024

void initialize_colors() {
    initscr();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
}

void cleanup_ncurses() {
    endwin();
}

void print_prompt() {
    attron(COLOR_PAIR(1));
    printf("myShell> ");
    attroff(COLOR_PAIR(1));
    fflush(stdout);
}

void get_user_input(char *input_buffer) {
    attron(COLOR_PAIR(2));
    fgets(input_buffer, MAX_CMD_LENGTH, stdin);
    attroff(COLOR_PAIR(2));
}

void motd() {
    printf("\033[1;32mWelcome to My Custom Shell!\033[0m\n");
}

void parseInput(char *input, char **args) {
    char *token = strtok(input, " \n");
    int i = 0;
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL; // Mark the end of the arguments
}

int executeCommand(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] != NULL) {
            chdir(args[1]);
        }
        return 1; // Command executed
    } else if (strcmp(args[0], "exit") == 0) {
        return 0; // Exit the shell
    }
    return -1; // Command needs to be executed
}

void runCommand(char **args, int background) {
    int pipe_fd[2];
    if (strchr(args[0], '|')) {
        char *cmd1[MAX_ARGS];
        char *cmd2[MAX_ARGS];
        int i = 0;

        // Split commands at the pipe symbol
        char *token = strtok(args[0], "|");
        while (token != NULL) {
            if (i == 0) {
                cmd1[i++] = token;
            } else {
                cmd2[i - 1] = token; // Put the second command in the second array
            }
            token = strtok(NULL, "|");
        }
        cmd1[i] = NULL; // Terminate first command
        cmd2[i - 1] = NULL; // Terminate second command

        // Create the pipe
        if (pipe(pipe_fd) == -1) {
            perror("pipe failed");
            return;
        }

        pid_t pid1 = fork();
        if (pid1 == 0) {
            // First child process
            dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to pipe
            close(pipe_fd[0]);
            execvp(cmd1[0], cmd1);
            perror("Error executing command 1");
            exit(EXIT_FAILURE);
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {
            // Second child process
            dup2(pipe_fd[0], STDIN_FILENO); // Redirect stdin to pipe
            close(pipe_fd[1]);
            execvp(cmd2[0], cmd2);
            perror("Error executing command 2");
            exit(EXIT_FAILURE);
        }

        close(pipe_fd[0]);
        close(pipe_fd[1]);

        if (!background) {
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
    } else {
        // Handle single command execution as before
        pid_t pid = fork();
        if (pid == 0) {
            if (execvp(args[0], args) == -1) {
                perror("Error executing command");
            }
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            if (!background) {
                printf("-------------------- Starting program --------------------------\n");
                wait(NULL);
                printf("-------------------- Program ended -----------------------------\n");
            }
        } else {
            perror("Fork failed");
        }
    }
}

int isBackground(char *input) {
    if (input[strlen(input) - 1] == '\n') {
        input[strlen(input) - 1] = '\0';
    }

    if (input[strlen(input) - 1] == '&') {
        input[strlen(input) - 1] = '\0';
        return 1;
    }
    return 0;
}

int main() {
    char input[MAX_CMD_LENGTH];
    char *args[MAX_ARGS];
    int background;
    initialize_colors();
    motd();

    while (1) {
        print_prompt();
        get_user_input(input);

        background = isBackground(input);
        parseInput(input, args);

        int status = executeCommand(args);
        if (status == 0) break; // Exit command
        else if (status == -1) runCommand(args, background);
    }

    cleanup_ncurses();
    return 0;
}
