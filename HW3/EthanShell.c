#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ncurses.h>   // Include ncurses library for color handling

void initialize_colors() {
    initscr();            // Start ncurses mode
    cbreak();             // Disable line buffering
    noecho();             // Don't echo input by default
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
    printw("myShell> ");   // Use printw instead of printf
    attroff(COLOR_PAIR(1)); // Turn off the color
    refresh();             // Refresh to show the output
}

void get_user_input(char *input_buffer, size_t size) {
    attron(COLOR_PAIR(2));  // Turn on the user input color (cyan)
    getnstr(input_buffer, size - 1);  // Get user input, use getnstr for ncurses
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
        print_prompt();  // Changed to print_prompt
        get_user_input(input, sizeof(input));  // Get user input

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
    attron(COLOR_PAIR(1));  // Set the color for the message
    printw("Welcome to My Custom Shell!\n");
    attroff(COLOR_PAIR(1));  // Reset the color
    refresh();  // Refresh to show the message
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
            printw("-------------------- Starting program --------------------------\n");
            refresh();  // Refresh to show the output
            wait(NULL);
            printw("-------------------- Program ended -----------------------------\n");
            refresh();  // Refresh to show the output
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
