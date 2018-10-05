EXEC_FILE = assembler
C_FILES = main.c lexer.c utility.c parsing.c files.c lables.c commands.c
H_FILES = assembler.h

O_FILES = $(C_FILES:.c=.o)

all: $(EXEC_FILE)
$(EXEC_FILE): $(O_FILES)
	gcc -Wall -ansi -pedantic -g $(O_FILES) -o $(EXEC_FILE)
%.o: %.c $(H_FILES)
	gcc -Wall -ansi -pedantic -c -o $@ $<
clean:
	rm -f *.o $(EXEC_FILE)
