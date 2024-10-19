CC = gcc
CFLAGS = -Wall -Wextra -lncurses  # Link with ncurses library
TARGET = EthanShell

all: $(TARGET)

$(TARGET): $(TARGET).o
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).o

$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) -c $(TARGET).c

clean:
	rm -f *.o $(TARGET)

install: $(TARGET)
	sudo cp $(TARGET) /bin/

uninstall:
	sudo rm /bin/$(TARGET)
