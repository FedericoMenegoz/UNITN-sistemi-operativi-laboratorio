SHELL := /bin/bash
all: sol test



.PHONY: all sol test

test:
	@$(MAKE) -C test
	@mv ./test/test test.out
sol:
	@$(MAKE) -C sol

clean:
	@$(MAKE) -C sol clean

info:
	@echo "************************************************************"
	@echo ""
	@echo "1. Replace your solutions to the '/sol' folder"
	@echo "2. make FILE=log.txt "
	@echo "3. ./test.out log.txt "
	@echo ""
	@echo "make clean-target (remove binaries)"
	@echo "make clean FILE=log.txt (remove log.txt and SignalProxy.out)"
	@echo ""
	@echo "************************************************************"