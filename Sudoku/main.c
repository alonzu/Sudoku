#include <stdio.h>
#include <stdlib.h>
#include "mainAux.h"
#include "parser.h"
#include "commands.h"
#include "linkedList.h"
#include "gurobi_c.h"
#include <time.h>


int main() {
    srand(time(NULL));
    printf("Sudoku\n------\n");
    start_game(); /* starts the game */
    return 0;
}
