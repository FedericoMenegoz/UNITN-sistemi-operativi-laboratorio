SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic

ALL:
	$(MAKE) -C sol compila
	$(MAKE) -C test
commit:
	$(MAKE) -C sol commit
.PHONY: ALL