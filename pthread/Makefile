SHELL :=  /bin/bash

DIRECTORIES := $(wildcard */)


all: $(DIRECTORIES)

$(DIRECTORIES):
	@$(MAKE) -C $@

.PHONY: all $(DIRECTORIES)

%:
	@echo "Creating new dir $@ ..."
	@[[ ! -e $@ ]] && mkdir $@ || echo "Error ? $@ exists "
	@echo "Creating files ..."
	@[[ -d $@ ]] && touch $@/main.c && cp counter/Makefile $@/ && echo "All done." || echo "Something went wrong... oh well shit happens right?"
