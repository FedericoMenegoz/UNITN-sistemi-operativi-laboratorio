SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic

default:
	@echo "To build please type 'make compila'"

compila: signal_handler.out
	@echo "$< built."

signal_handler.out: signal_handler.c
	@echo "Building $@"
	@$(CC) $(CFLAGS) $< -o $@

commit: 
ifeq ($(COMMIT),)
	@echo "COMMIT ?"
else
	@git add .
	@git commit -m "202401 - $(COMMIT)"
endif
.PHONY: compila commit

