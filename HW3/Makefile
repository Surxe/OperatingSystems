CC = gcc
CFLAGS = -Wall -Wextra -c  # Compile flags
LDFLAGS = -lncurses        # Linker flags for ncurses
TARGET = EthanShell

all: $(TARGET)

$(TARGET): $(TARGET).o
    $(CC) -o $(TARGET) $(TARGET).o $(LDFLAGS)  # Link with ncurses library

$(TARGET).o: $(TARGET).c
    $(CC) $(CFLAGS) $(TARGET).c

clean:
    rm -f *.o $(TARGET)

install: $(TARGET)
    sudo cp $(TARGET) /bin/

uninstall:
    sudo rm /bin/$(TARGET)
