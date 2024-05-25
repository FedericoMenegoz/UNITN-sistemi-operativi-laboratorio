SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall -pthread

TARGET := SignalProxy.out
FILE := log.txt

ALL: $(TARGET) $(FILE)
	@echo "$^ built."

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

commit: 
ifeq ($(strip $(COMMIT)),)
	@echo "Error: make commit COMMIT=<commitName>"
else
	@git add . && git commit -m "202301 - $(COMMIT)"
endif
.PHONY: clean ALL commit