# Makefile for compiling memory_info.c with OS-specific flags

# Compiler
CC = gcc

# Executable name
TARGET = SysInfoC

# Source files
SRC = SysInfo.c

# Default target
all: $(TARGET)

# Detect the OS and set flags
ifeq ($(shell uname), Darwin)
    # macOS-specific settings
    CFLAGS = -framework CoreFoundation -framework IOKit
else
    # Settings for other OS (adjust as needed)
    CFLAGS = 
endif

# Compile the target
$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS)

# Clean up build files
clean:
	rm -f $(TARGET)
