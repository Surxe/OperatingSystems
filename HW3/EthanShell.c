#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ncurses.h>   // Include ncurses library for color handling

void initialize_colors() {
    initscr();            // Start ncurses mode
    start_color();        // Enable color functionality

    // Define color pairs for user input and prompt
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  // Prompt color: green on black
    init_pair(2, COLOR_CYAN, COLOR_BLACK);   // User input color: cyan on black
}

void cleanup_ncurses() {
    endwin();   // Exit ncurses mode
}

void print_prompt() {
    attron(COLOR_PAIR(1)); // Turn on the prompt color (green)
    printf("myShell> ");
    attroff(COLOR_PAIR(1)); // Turn off the color
    fflush(stdout);         // Make sure the prompt is printed
}

void get_user_input(char *input_buffer) {
    attron(COLOR_PAIR(2));  // Turn on the user input color (cyan)
    fgets(input_buffer, 1024, stdin);  // Get user input
    attroff(COLOR_PAIR(2));  // Turn off the color after input
}

// Function declarations
void motd();
void prompt();
void parseInput(char *input, char **args);
int executeCommand(char **args);
void runCommand(char **args, int background);
int isBackground(char *input);

int main() {
    char input[1024];
    initialize_colors();
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

    cleanup_ncurses();
    
    return 0;
}

// Implement your functions here...

void motd() {
    printf("\033[1;32mWelcome to My Custom Shell!\033[0m\n");
}

void prompt() {
    printf("\033[1;34mEthanShell$\033[0m ");
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

