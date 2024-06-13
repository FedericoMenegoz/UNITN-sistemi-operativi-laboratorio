SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic

SOURCES := $(wildcard *.c)
TARGETS := $(patsubst %.c, %, $(filter-out lib.c, $(SOURCES)))

ALL: $(TARGETS)

$(TARGETS): %: %.c
	@echo "Building targets: $@"
	@$(CC) $(CFLAGS) $^ -o $@ && echo "Built succesfull."

clean:
	@echo "Removing $(TARGETS)" 
	@rm $(TARGETS) 2>/dev/null || echo "Nothing to do."


.PHONY: clean ALL