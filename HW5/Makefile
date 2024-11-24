# Compiler and flags
CXX := g++
CXXFLAGS := -pthread -lrt -Wall -std=c++11

# Source and target files
SRC := main.cpp
TARGET := pthread_semaphore

# Default build rule
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Clean up the compiled files
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean
