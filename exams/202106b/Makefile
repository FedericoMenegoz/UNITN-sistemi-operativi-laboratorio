SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall
FOLDER := /tmp/info

ALL: app

app: main.c
	@echo "Building app ..."
	@$(CC) $(CFLAGS) $< -o $@
	@echo "App builded."

.PHONY: clean ALL

clean:
	rm -r $(FOLDER)