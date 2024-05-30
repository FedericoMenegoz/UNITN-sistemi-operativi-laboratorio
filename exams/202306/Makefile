SHELL := /bin/bash
all: sol test

.PHONY: all sol test

test:
	@$(MAKE) -C test
	@mv ./test/test.out test.out
sol:
	@$(MAKE) -C sol 

clean:
	@$(MAKE) -C sol clean
	@$(MAKE) -C test clean

info:
	@echo "************************************************************"
	@echo ""
	@echo "1. Replace your solutions to the '/sol' folder"
	@echo "2. make FILE=/tmp/esame202306_cmd "
	@echo "3. ./test.out "
	@echo ""
	@echo "make clean-target (remove binaries)"
	@echo "make clean"
	@echo ""
	@echo "************************************************************"