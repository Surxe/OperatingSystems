# MakeFile

CC = gcc
CFLAGS = -Wall -g
TARGET = luckygame
OBJ = LuckyNumber.o

all: $(TARGET)

$(TARGET): main.o $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) main.o $(OBJ)

LuckyNumber.o: LuckyNumber.c LuckyNumber.h
	$(CC) $(CFLAGS) -c LuckyNumber.c

main.o: main.c Record.h LuckyNumber.h
	$(CC) $(CFLAGS) -c main.c

install:
	sudo cp $(TARGET) /usr/local/bin/

uninstall:
	sudo rm /usr/local/bin/$(TARGET)

clean:
	rm -f *.o $(TARGET)
