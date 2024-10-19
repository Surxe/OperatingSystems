#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ncurses.h>

void initialize_colors() {
    initscr();            // Start ncurses mode
    cbreak();             // Disable line buffering
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
    echo();                 // Enable echo to display user input
    getnstr(input_buffer, size - 1);  // Get user input
    attroff(COLOR_PAIR(2));  // Turn off the color after input
    noecho();               // Disable echo again
}

// Function declarations
void motd();
void prompt();
void parseInput(char *input, char **args);
int executeCommand(char **args);
void runCommand(char **args, int background);
void runPipeCommand(char *input);
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

        // Handle pipe commands
        if (strstr(input, "|") != NULL) {
            runPipeCommand(input);
            continue;
        }

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

void runPipeCommand(char *input) {
    int pipefd[2]; // Array to hold the pipe file descriptors
    pid_t pid1, pid2;

    // Split the input on the pipe symbol
    char *command1 = strtok(input, "|");
    char *command2 = strtok(NULL, "|");

    if (pipe(pipefd) == -1) {
        perror("Pipe failed");
        return;
    }

    // First command
    if ((pid1 = fork()) == 0) {
        // Child process for command 1
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(pipefd[0]); // Close unused read end
        close(pipefd[1]); // Close write end after duplicating
        char *args1[64];
        parseInput(command1, args1); // Parse first command
        execvp(args1[0], args1);
        perror("Error executing first command");
        exit(EXIT_FAILURE);
    }

    // Second command
    if ((pid2 = fork()) == 0) {
        // Child process for command 2
        dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to pipe
        close(pipefd[1]); // Close unused write end
        close(pipefd[0]); // Close read end after duplicating
        char *args2[64];
        parseInput(command2, args2); // Parse second command
        execvp(args2[0], args2);
        perror("Error executing second command");
        exit(EXIT_FAILURE);
    }

    // Parent process
    close(pipefd[0]); // Close read end in parent
    close(pipefd[1]); // Close write end in parent

    // Wait for both child processes to finish
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
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
