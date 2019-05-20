all: sudoku-console
CC = gcc 
OBJS = main.o parser.o commands.o mainAux.o linkedList.o stack.o
 
EXEC = sudoku-console
COMP_FLAGS = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c mainAux.h parser.h commands.h linkedList.h gurobi_c.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
parser.o: parser.c mainAux.h commands.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
commands.o: commands.c commands.h mainAux.h linkedList.h gurobi_c.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
mainAux.o: mainAux.c linkedList.h stack.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
linkedList.o: linkedList.h mainAux.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
stack.o: stack.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
clean:
	rm -f *.o $(EXEC)
all: $(EXEC)
