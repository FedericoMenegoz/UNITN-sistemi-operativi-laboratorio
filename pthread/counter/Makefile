SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic
TARGET := counter.out

ALL: $(TARGET)
	@echo "$(TARGET) built."

$(TARGET): main.c
	@echo "Building $(TARGET) ..."
	@$(CC) $(CFLAGS) $< -o $@ -pthread

clean: 
	@[[ -e $(TARGET) ]] && rm $(TARGET) && echo "$(TARGET) removed" || echo "Nothing to do."
	