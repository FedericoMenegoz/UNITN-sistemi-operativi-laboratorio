SHELL:=/bin/bash
FILE:=../test/cmd.txt
EXE:=a.out
CC:=gcc
CFLAGS:=-Wall -Wpedantic -Wextra -pthread

ALL: $(EXE)
	@[ ! -e $(EXE) ] || echo "All done."

$(EXE): main.c
	@[[ ! -e $(FILE) ]] && echo "Error: $(FILE) not found." && exit 1 || echo "Compiling $@ ..."
ifneq ($(DIR),)
	@mkdir $(DIR)
	@$(CC) $(CFLAGS) $< -o $(DIR)/$@ && echo "Compilation success." || echo "Compilation failed."
else 
	@$(CC) $(CFLAGS) $< -o $@ && echo "Compilation success." || echo "Compilation failed."
endif

clean:
	@rm *.out 2>/dev/null || echo "'.out' exec not found."
ifneq ($(DIR),)
	@rm -r $(DIR)
endif

.PHONY: ALL