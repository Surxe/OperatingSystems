CC = gcc
CFLAGS = -Wall -Wextra
TARGET = EthanShell

all: $(TARGET)

$(TARGET): $(TARGET).o
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).o

$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) -c $(TARGET).c

clean:
	rm -r *.o $(TARGET)

install:
	sudo cp $(TARGET) /bin/

uninstall: sudo rm /bin/$(TARGET)
