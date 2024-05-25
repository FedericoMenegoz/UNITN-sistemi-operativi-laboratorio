SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall

TARGET := SignalProxy.out

ALL: $(TARGET) $(FILE) processA.out
	@echo "All built."

$(TARGET): main.c
	@echo "Building SignalProxy.out ..."
	@$(CC) $(CFLAGS) main.c -o SignalProxy.out

$(FILE):
	@[[ ! -e $(FILE) ]] && echo "start" >> $(FILE) || echo "$(FILE) exists ..."

clean: 
	@echo "Removing $(FILE) ..."
	@[[ -e $(FILE) ]] && rm $(FILE) 
	@echo "Removing $(TARGET) ..."
	@[[ -e $(TARGET) ]] && rm $(TARGET) 

.PHONY: clean ALL

processA.out: processA.c
	@$(CC) $(CFLAGS) $< -o $@
