SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic

SOURCES := $(wildcard *.c)
TARGETS := $(patsubst %.c, %, $(SOURCES))

ALL: $(TARGETS)

$(TARGETS): %: %.c
	@echo "Building targets: $@"
	@$(CC) $(CFLAGS) $^ -o $@ && echo "Built succesfull."

clean-target:
	@echo "Removing $(TARGETS)" 
	@rm $(TARGETS)

.PHONY: clean-target