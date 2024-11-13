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
    // Array to track whether a task has been printed
    int printed[len];  // Array to mark tasks as printed
    for (int i = 0; i < len; i++) {
        printed[i] = 0;  // Initialize to 0 (not printed)
    }

    // Iterate over scheduled task names
    for (int i = 0; i < len; i++) {
        const char *task = schedule[i];

        // Ignore empty task names and tasks that have already been printed
        if (strcmp(task, "") == 0 || printed[i]) {
            continue;
        }

        // Mark the task as printed
        printed[i] = 1;

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
