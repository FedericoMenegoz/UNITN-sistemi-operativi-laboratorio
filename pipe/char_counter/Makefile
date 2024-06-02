SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic
TARGET := char_counter.out 

ALL: $(TARGET)
	@echo "$< built."

$(TARGET): main.c
	@$(CC) $(CFLAGS) $< -o $@

clean: 
	@[[ -e $(TARGET) ]] && rm $(TARGET) && echo $(TARGET) removed. || echo Nothing to do.

.PHONY: clean ALL