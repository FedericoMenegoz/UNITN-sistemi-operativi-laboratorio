SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall
DEST := .

ALL: coda
	@echo "All done."

coda: main.c
	@echo "Building $@ ..."
	@$(CC) $(CFLAGS) $< -o $(DEST)/$@ || echo "?ERROR"

.PHONY: ALL

%.out: %.c
	@$(CC) $(CFLAGS) $< -o $@ && echo "Aux receiver done."