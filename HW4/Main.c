#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a Task structure
typedef struct {
    const char *name;
    int arrival_time;
    int burst_duration;
} Task;

// Function to compare two strings for alphabetical order
int compare_task_names(const void *a, const void *b) {
    return strcmp(((Task *)a)->name, ((Task *)b)->name);
}

// FIFO scheduling function
char** schedule_FIFO(Task tasks[], int num_tasks) {
    // Initialize an array to store the schedule output
    char **schedule = (char **)malloc(num_tasks * sizeof(char *));
    for (int i = 0; i < num_tasks; i++) {
        schedule[i] = (char *)malloc(100 * sizeof(char)); // Assuming 100 chars are enough for each task
        memset(schedule[i], 'x', 99);  // Initialize with 'x' (tasks not yet arrived)
        schedule[i][99] = '\0'; // Null-terminate each schedule string
    }

    // Sort tasks by arrival time and name if arrival times are the same
    qsort(tasks, num_tasks, sizeof(Task), compare_task_names);

    // Simulate the scheduling process over time
    int current_time = 0; // Current time tick
    int task_index = 0;   // Index for tasks to be executed
    int tasks_completed = 0; // Counter for completed tasks

    while (tasks_completed < num_tasks) {
        // Check for available tasks
        for (int i = task_index; i < num_tasks; i++) {
            if (tasks[i].arrival_time <= current_time) {
                // The task has arrived and can be scheduled
                int len = strlen(schedule[i]);
                schedule[i][current_time - tasks[i].arrival_time] = '#'; // Mark task execution
                task_index = i + 1; // Move to the next task
            }
        }

        // After checking all tasks, increment the time
        current_time++;
        tasks_completed++;
    }

    return schedule;
}

void free_schedule(char **schedule, int num_tasks) {
    for (int i = 0; i < num_tasks; i++) {
        free(schedule[i]);
    }
    free(schedule);
}

int main() {
    // Define an array of tasks
    Task tasks[] = {
        {"Task1", 5, 1},
        {"Task2", 0, 2},
        {"Task3", 1, 1}
    };
    
    int num_tasks = sizeof(tasks) / sizeof(tasks[0]);

    // Get the FIFO schedule
    char **schedule = schedule_FIFO(tasks, num_tasks);

    // Print the schedule output for each task
    for (int i = 0; i < num_tasks; i++) {
        printf("%s: %s\n", tasks[i].name, schedule[i]);
    }

    // Free the dynamically allocated memory for the schedule
    free_schedule(schedule, num_tasks);

    return 0;
}
