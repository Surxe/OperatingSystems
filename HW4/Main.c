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
	//Iterate scheduled task names
	for (int i=0; i< len; i++) {
		const char *task = schedule[i];
		char task_string[100] = "I"; //A task's string consisting of symbols # and _
		//Assume 100 is sufficient string length		

		//Iterate schedule task names again
		//Append a symbol at a time to the task string for each time its in the task
		for (int j=0; j < len; j++) {
			
			if (1==1) {
				//Append the symbol
				strcat(task_string, "#");
			}
		}

		printf("%s: %s\n", schedule[i], task_string);
	}
}

int main() {
	//Use some input
	Task input[] = {
		{"Task1Name", 0, 5},
		{"Task2Name", 1, 6},
		{"Task3Name", 2, 7},
	};

	int len = sizeof(input) / sizeof(input[0]);	
	printf("Scheduling %d tasks\n\n", len);
	

	print_tasks(input, len);
	printf("\n");

	const char *schedule[] = {
		"Task1Name",
		"Task2Name",
		"Task3Name",
	};

	int len_schedule = sizeof(schedule) / sizeof(schedule[0]);

	printf("Schedule for (PLACEHOLDER)\n");
	print_schedule(schedule, len_schedule);

	return 0;
}
