#include <stdio.h>

typedef struct {
	const char *name;
	int arrival_time;
	int burst_duration;
} Task;

int main() {
	//Use some input
	Task input[] = {
		{"Task1Name", 0, 5},
		{"Task2Name", 1, 6},
		{"Task3Name", 2, 7},
	};

	int len = sizeof(input);	
	printf("Testing");
	printf("%d", len);
	return 0;
}
