# program1/Makefile (similar for program2/Makefile, etc.)

# Variables
CC = gcc
CFLAGS = -Wall -g -I../include
MAIN_SRC = main.c
MAIN_OBJ = main.o
UTILS_OBJ = ../include/utils.o
TARGET = hospital.out  # Change this for each program directory

# Target to create the executable
$(TARGET): $(MAIN_OBJ) $(UTILS_OBJ)
	$(CC) $(CFLAGS) $(MAIN_OBJ) $(UTILS_OBJ) -o $(TARGET)

# Compile main.c to main.o
$(MAIN_OBJ): $(MAIN_SRC) ../include/utils.h
	$(CC) $(CFLAGS) -c $(MAIN_SRC) -o $(MAIN_OBJ)

# Clean target to remove generated files
.PHONY: clean
clean:
	rm -f $(MAIN_OBJ) $(TARGET)
