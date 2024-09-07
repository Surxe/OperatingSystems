#include <stdio.h>
#include "Record.h"
#include "LuckyNumber.h"

int main() {
	Record playerRecord = {0, 0, 0};
	int guess, diff;
	char playAgain;

	//Seed
	srand(time(0));

	do {
		//Request user input
		printf("Guess a number from 1 to 5: ");
		scanf("%d", &guess);

		//Ensure input is in valid range
		if (guess > 5 || guess < 1) {
			printf("Invalid guess, must be between 1 and 5.\n");
			continue;
		}

		//Check guess
		diff = checkLuckyNumber(guess);

		if (diff == 0) {
			playerRecord.won++;
		} else if (diff < 0) {
			playerRecord.lessThan++;
		} else if (diff > 0) {
			playerRecord.greaterThan++;
		}

		//Show record
		printf("=: %d, <: %d, >: %d\n", playerRecord.won, playerRecord.lessThan, playerRecord.greaterThan);

		//Continue loop
		printf("Play again..? (y/n): ");
		scanf(" %c", &playAgain);

	}
	while (playAgain == 'Y' || playAgain == 'y');

	printf("Program complete");

	return 0;
}
