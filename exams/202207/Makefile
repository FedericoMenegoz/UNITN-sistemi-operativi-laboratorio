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
	@echo "4. ./test.out"
	@echo ""
	@echo "make clean-target (remove binaries)"
	@echo "make clean (remove log.txt and SignalProxy.out)"
	@echo ""
	@echo "To specify a custom log file type FILE=<your/fav/path>"
	@echo ""
	@echo "************************************************************"