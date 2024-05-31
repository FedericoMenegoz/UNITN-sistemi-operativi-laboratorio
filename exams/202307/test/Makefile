SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic
OUT_FILES := $(wildcard *.out)
SOURCES := $(wildcard *.c)
TARGET := $(patsubst %.c, %.out, $(SOURCES))

ALL: $(TARGET)
	@echo $(TARGET) built ...

.PHONY: clean ALL

%.out: %.c
	@echo building $@ ...
	@$(CC) $(CFLAGS) $< -o $@ -pthread

clean: 
	@echo removing $(OUT_FILES)
	@rm $(OUT_FILES) 