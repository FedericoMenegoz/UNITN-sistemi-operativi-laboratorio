SHELL := /bin/bash
all: sol test



.PHONY: all sol test

test:
	@$(MAKE) -C test

sol:
	@$(MAKE) -C sol

clean:
	@$(MAKE) -C sol clean
	@$(MAKE) -C test clean

info:
	@echo "************************************************************"
	@echo ""
	@echo "1. Replace your solutions to the '/sol' folder"
	@echo "2. make clean"
	@echo "3. make"
	@echo "4. ./test.sh"
	@echo ""
	@echo "make clean (remove binaries)"
	@echo ""
	@echo "************************************************************"