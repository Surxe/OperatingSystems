CXX := g++
CXXFLAGS := -pthread -lrt -Wall -std=c++11

SRC := main.cpp
TARGET := pthread_semaphore

# default build rule
all: $(TARGET)

# build target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
