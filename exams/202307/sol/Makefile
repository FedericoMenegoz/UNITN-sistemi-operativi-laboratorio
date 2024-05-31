SHELL := /bin/bash
CC := gcc
CFLAGS := -Wall -Wpedantic -Wextra

PostOffice: main.c
	@echo "Building $@ ..."
	@$(CC) $(CFLAGS) $< -o $@
	@echo "$@ built."

clean:
	@[[ -e PostOffice ]] && @echo "Removing PostOffice." && rm PostOffice || echo "Nothing to do ..."

commit:
ifeq ($(COMMIT),)
	@echo "make commit COMMIT=<name>"
else
	git add . && git commit -m "202307 - $(COMMIT)" && echo "Commit 202307 - $(COMMIT) done." || echo "ERROR?"
endif