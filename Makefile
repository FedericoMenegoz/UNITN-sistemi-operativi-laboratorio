# Variables
CC = gcc
CFLAGS = -Wall -g -I./include
UTILS_SRC = ./include/utils.c
UTILS_OBJ = ./include/utils.o
PROGRAMS = hospital

# Target to build all programs
all: $(PROGRAMS)

# Pattern rule to build each program
$(PROGRAMS):
	$(MAKE) -C $@

# Target to build the object file for utils
$(UTILS_OBJ): $(UTILS_SRC)
	$(CC) $(CFLAGS) -c $(UTILS_SRC) -o $(UTILS_OBJ)

# Clean target to remove generated files
.PHONY: clean $(PROGRAMS)
clean:
	rm -f $(UTILS_OBJ)
	for dir in $(PROGRAMS); do \
		$(MAKE) -C $$dir clean; \
	done
