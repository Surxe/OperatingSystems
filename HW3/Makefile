CC = gcc
CFLAGS = -Wall -Wextra -c    # Compile flags
LDFLAGS = -lncurses          # Linker flags for ncurses
TARGET = EthanShell

# Default target
all: $(TARGET)

# Link the final executable
$(TARGET): $(TARGET).o
	$(CC) -o $(TARGET) $(TARGET).o $(LDFLAGS)  # Link with ncurses library

# Compile the source file to object file
$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c

# Clean up build artifacts
clean:
	rm -f *.o $(TARGET)

# Install the binary to /bin/
install: $(TARGET)
	sudo cp $(TARGET) /bin/

# Uninstall the binary from /bin/
uninstall:
	sudo rm /bin/$(TARGET)
