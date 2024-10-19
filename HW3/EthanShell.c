#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Function declarations
void motd();
void prompt();
void parseInput(char *input, char **args);
int executeCommand(char **args);
void runCommand(char **args, int background);
int isBackground(char *input);

int main() {
    char input[1024];
    char *args[64];
    int background;
    motd();

    while (1) {
        prompt();
        fgets(input, sizeof(input), stdin);
        
        background = isBackground(input);
        parseInput(input, args);
        
        int status = executeCommand(args);
        if (status == 0) break; // Exit command
        else if (status == -1) runCommand(args, background);
    }

    return 0;
}

// Implement your functions here...

void motd() {
    printf("\033[1;32mWelcome to My Custom Shell!\033[0m\n");
}

void prompt() {
    printf("\033[1;34mmy-shell$\033[0m ");
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
        chdir(args[1]);
        return 1;
    } else if (strcmp(args[0], "exit") == 0) {
        return 0; // Exit the shell
    }
    return -1; // Command needs to be executed by exec
}

void runCommand(char **args, int background) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("Error executing command");
        }
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        if (!background) {
            // If it's not a background process, wait for it to finish
            printf("-------------------- Starting program --------------------------\n");
            wait(NULL);
            printf("-------------------- Program ended -----------------------------\n");
        }
    } else {
        perror("Fork failed");
    }
}


int isBackground(char *input) {
    // Remove trailing newline character
    if (input[strlen(input) - 1] == '\n') {
        input[strlen(input) - 1] = '\0';
    }

    // Check if the last character is '&'
    if (input[strlen(input) - 1] == '&') {
        input[strlen(input) - 1] = '\0';  // Remove '&' from the input
        return 1;
    }
    return 0;
}

