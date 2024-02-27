# Target executable name
TARGET = part_III.out

# Source files
SRCS = bfs.c main.c ThreadPool/* SyncGraph/* MyQueue/*

# Compiler and flags
CC = gcc
CFLAGS = -lpthread

# Default target builds the executable
$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) -o $@

# Clean target removes the executable
clean:
	rm -f $(TARGET)

