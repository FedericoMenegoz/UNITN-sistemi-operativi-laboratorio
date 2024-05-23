SHELL := /bin/bash

CC := gcc
CFLAGS := -Wall -g

SOURCES := $(wildcard *.c)
TARGET := $(patsubst %.c, %, $(SOURCES))
OBJECT := $(patsubst %.c, %.o, $(SOURCES))

ALL: $(TARGET)

%: %.o lib.o
	$(CC) $(CFLAGS) $^ -o $@

lib.o: lib.c lib.h
	$(CC) $(CFLAGS) -c lib.c


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: ALL


