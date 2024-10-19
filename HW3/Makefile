CC = gcc
CFLAGS = -Wall -Wextra -c  # Compile flags (just compile, no linking)
LDFLAGS = -lncurses        # Linker flags for ncurses
TARGET = EthanShell

all: $(TARGET)

$(TARGET): $(TARGET).o
	$(CC) -o $(TARGET) $(TARGET).o $(LDFLAGS)  # Link with ncurses library here

$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c

clean:
	rm -f *.o $(TARGET)

install: $(TARGET)
	sudo cp $(TARGET) /bin/

uninstall:
	sudo rm /bin/$(TARGET)
