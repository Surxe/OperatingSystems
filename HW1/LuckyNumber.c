#include "LuckyNumber.h" //get prototype from header

int checkLuckyNumber(int guess) {
	int randomNumber = (rand()%5)+1; //0-4 to 1-5


	//guess<randomNumber returns negative
	//guess>randomNumber returns positive
	//guess=randomNumber returns 0
	return guess-randomNumber;
} 
