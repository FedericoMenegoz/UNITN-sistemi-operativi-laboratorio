SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall -Wpedantic -Wextra
DIR := out
EXE := a.out

ALL: main.c
ifeq ($(FILE),)
	@echo "ERROR? FILE missing"
else
	@[[ -e $(FILE) ]] && mkdir $(DIR) && cp $(FILE) $(DIR) && $(CC) $(CFLAGS) main.c -o $(DIR)/$(EXE) -pthread || echo $(FILE) does not exists.
endif


.PHONY: ALL clean

clean:
	@[[ -d $(DIR) ]] && rm -r $(DIR) && echo "$(DIR) removed" || echo "nothing to do"
