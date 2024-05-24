SHELL := /bin/bash

all: sol test

.PHONY: all sol test

test:
	@$(MAKE) -C test
	@cp ./test/test test.out
sol:
	@$(MAKE) -C sol

clean-target:
	@$(MAKE) -C sol clean-target
	@$(MAKE) -C test clean-target
	@rm test.out

info:
	@echo "replace your solutions to the '/sol' folder"
	@echo "type make ..."
	@echo "run ./test.out"
