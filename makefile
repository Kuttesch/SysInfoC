# Makefile for compiling SysInfo.c with OS-specific flags

# Compiler
CC = gcc

# Executable name
TARGET = SysInfoC

# Source files
SRC = SysInfoC.c

# Default target
all: $(TARGET)

# Detect the OS and set flags
ifeq ($(OS), Windows_NT)
    # Windows-specific settings
    CFLAGS = 
else ifeq ($(shell uname), Darwin)
    # macOS-specific settings
    CFLAGS = -framework CoreFoundation -framework IOKit
else
    # Default settings for other systems
    CFLAGS = 
endif

# Compile the target
$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS)

# Clean up build files
clean:
	rm -f $(TARGET)
