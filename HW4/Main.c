#include <stdio.h>
#include <string.h>

typedef struct {
	const char *name;
	int arrival_time;
	int burst_duration;
} Task;

void print_tasks(Task tasks[], int num_tasks) {
	printf("Tasks:\n");
	for (int i=0; i < num_tasks; i++) {
		printf("Task Name: %s, Arrival Time: %d, Burst Duration %d\n",
			tasks[i].name, tasks[i].arrival_time, tasks[i].burst_duration);
	}
}

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

int main() {
	//Use some input
	Task input[] = {
		{"Task1Name", 0, 1},
		{"Task2Name", 1, 2},
		{"Task3Name", 3, 3},
	};

	int len = sizeof(input) / sizeof(input[0]);	
	printf("Scheduling %d tasks\n\n", len);
	

	print_tasks(input, len);
	printf("\n");

	const char *schedule[] = {
		"Task1Name",
		"Task2Name",
		"Task2Name",
		"",
		"Task3Name",
		"Task3Name",
		"Task3Name",
	};

	int len_schedule = sizeof(schedule) / sizeof(schedule[0]);

	printf("Schedule for (PLACEHOLDER)\n");
	print_schedule(schedule, len_schedule);

	return 0;
}
