#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the Task struct
typedef struct {
    const char *name;      // Task name
    int arrival_time;     // Time when the task arrives
    int burst_duration;   // Duration for which the task runs
} Task;

// Function to print tasks
void print_tasks(Task tasks[], int num_tasks) {
    printf("Tasks:\n");
    for (int i = 0; i < num_tasks; i++) {
        printf("Task Name: %s, Arrival Time: %d, Burst Duration: %d\n",
               tasks[i].name, tasks[i].arrival_time, tasks[i].burst_duration);
    }
}

// Function to print the schedule
void print_schedule(const char *schedule[], int len) {
    // Create a list to store already printed task names
    char printed_names[len][100];  // Assuming max task name length of 100 characters
    int printed_count = 0;  // Counter for how many tasks have been printed

    // Iterate over the scheduled task names
    for (int i = 0; i < len; i++) {
        const char *task = schedule[i];

        // Skip empty task names
        if (strcmp(task, "") == 0) {
            continue;
        }

        // Check if this task has already been printed
        int already_printed = 0;
        for (int j = 0; j < printed_count; j++) {
            if (strcmp(task, printed_names[j]) == 0) {
                already_printed = 1;
                break;
            }
        }

        // If the task has already been printed, skip it
        if (already_printed) {
            continue;
        }

        // Add this task to the printed names list
        strcpy(printed_names[printed_count], task);
        printed_count++;

        // Initialize the task string with a symbol
        char task_string[100] = "";  // A task's string consisting of symbols # and _

        // Iterate again to append a symbol at a time to the task string
        for (int j = 0; j < len; j++) {
            if (strcmp(schedule[i], schedule[j]) == 0) {
                // Task is scheduled for this tick
                strcat(task_string, "#");
            } else {
                // Task is not scheduled for this tick
                strcat(task_string, "_");
            }
        }

        // Print the task and its schedule string
        printf("%s: %s\n", task, task_string);
    }
}

// Compare function to sort tasks based on arrival time
int compare_tasks(const void *a, const void *b) {
    Task *taskA = (Task*)a;
    Task *taskB = (Task*)b;
    return taskA->arrival_time - taskB->arrival_time;
}

// Function to schedule tasks using FIFO (First-In-First-Out)
char** schedule_FIFO(Task tasks[], int num_tasks) {
    // Sort the tasks by arrival time
    qsort(tasks, num_tasks, sizeof(Task), compare_tasks);

    // Print the tasks details after sorting
    print_tasks(tasks, num_tasks);

    // Allocate memory for the list of task names
    char **scheduled_tasks = (char**)malloc(num_tasks * sizeof(char*));

    // Fill the list with task names in FIFO order
    for (int i = 0; i < num_tasks; i++) {
        scheduled_tasks[i] = (char*)malloc(strlen(tasks[i].name) + 1);  // +1 for null terminator
        strcpy(scheduled_tasks[i], tasks[i].name);
    }

    return scheduled_tasks;
}

// Function to free the memory allocated for the task list
void free_scheduled_tasks(char **scheduled_tasks, int num_tasks) {
    for (int i = 0; i < num_tasks; i++) {
        free(scheduled_tasks[i]);
    }
    free(scheduled_tasks);
}

int main() {
    // Example tasks
    Task tasks[] = {
        {"Task1", 2, 5},
        {"Task2", 0, 3},
        {"Task3", 1, 4}
    };
    
    int num_tasks = sizeof(tasks) / sizeof(tasks[0]);

    // Schedule the tasks using FIFO
    char **scheduled_tasks = schedule_FIFO(tasks, num_tasks);

    // Print the scheduled tasks in FIFO order
    printf("\nScheduled tasks in FIFO order:\n");
    print_schedule(scheduled_tasks, num_tasks);

    // Free the dynamically allocated memory
    free_scheduled_tasks(scheduled_tasks, num_tasks);

    return 0;
}
