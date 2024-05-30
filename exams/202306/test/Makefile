SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic

SOURCES := $(wildcard *.c)
TARGET := $(patsubst %.c, %.out, $(SOURCES))

ALL: $(TARGET)
	@echo $(TARGET) built ...

.PHONY: clean ALL

%.out: %.c
	@echo building $@ ...
	@$(CC) $(CFLAGS) $< -o $@ -pthread

clean: 
	@rm signal_receiver.out