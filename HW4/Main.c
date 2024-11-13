#include <stdio.h>

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

int main() {
	//Use some input
	Task input[] = {
		{"Task1Name", 0, 5},
		{"Task2Name", 1, 6},
		{"Task3Name", 2, 7},
	};

	int len = sizeof(input) / sizeof(input[0]);	
	printf("Testing\n");
	printf("Num tasks: %d\n", len);
	

	print_tasks(input, len);

	return 0;
}
