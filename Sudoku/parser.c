#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "mainAux.h"
#include "commands.h"
#include "linkedList.h"

#define maxCommandSize 256

/* parsing input from the user and addresses the relevant command method */
void readAndReactBuffer() {
    char command[maxCommandSize];
    char * token;
    char *xString, *yString, *zString;
    FILE * fp;
    double x, y, z;
    int validateFlag;
    x = y = z = -1;
    while(fgets(command, maxCommandSize, stdin) != NULL) {
        token = strtok(command, " \t\r\n");
        if (token != NULL && strlen(token) > 256) {
            printf("ERROR: invalid command\n");
            continue;
        }
        if (token != NULL) {
            if (strcmp(token, "hint") == 0) {
                xString = strtok(NULL, " \t\r\n");
                yString = strtok(NULL, " \t\r\n");
                if (xString == NULL || yString == NULL) {
                    printf("ERROR: invalid command\n");
                    printf("Enter your command:\n");
                    continue;
                }
                if (atoi(xString) == 0) {
                    printf("Error: value not in range 1-%d\n", N);
                    printf("Enter your command:\n");
                    continue;
                } else {
                    x = atof(xString);
                }
                if (atoi(yString) == 0) {
                    printf("Error: value not in range 1-%d\n", N);
                    printf("Enter your command:\n");
                    continue;
                } else {
                    y = atof(yString);
                }
                if (x < 1 || y < 1 || x > N || y > N || (x - (int) x) != 0 || (y - (int) y) != 0) {
                    printf("Error: value not in range 1-%d\n", N);
                    printf("Enter your command:\n");
                    continue;
                }
                hint((int)y, (int)x);
                printf("Enter your command:\n");
                continue;
            } else if (strcmp(token, "solve") == 0) {
                xString = strtok(NULL, " \t\r\n");
                if (xString == NULL) {
                    printf("ERROR: invalid command\n");
                    printf("Enter your command:\n");
                    continue;
                }
                fp = fopen(xString, "r"); /*open file in reading mode*/
                if (fp == NULL) {
                    printf("Error: File doesn't exist or cannot be opened\n");
                    printf("Enter your command:\n");
                    continue;
                }
                fclose(fp);
                solve(xString);
            } else if (strcmp(token, "edit") == 0) {
                xString = strtok(NULL, " \t\r\n");
                if (xString == NULL) {
                    edit(xString);
                    printf("Enter your command:\n");
                    continue;
                }
                fp = fopen(xString, "r"); /*open file in reading mode*/
                if (fp == NULL) {
                    printf("Error: File cannot be opened\n");
                    printf("Enter your command:\n");
                    continue;
                }
                fclose(fp);
                edit(xString);
            } else if (strcmp(token, "mark_errors") == 0) {
                xString = strtok(NULL, " \t\r\n");
                if (xString == NULL) {
                    printf("ERROR: invalid command\n");
                    printf("Enter your command:\n");
                    continue;
                }
                if (strcmp(xString, "0") != 0 && atoi(xString) == 0) {
                    printf("Error: the value should be 0 or 1\n");
                    printf("Enter your command:\n");
                    continue;
                }
                x = atof(xString);
                if ((x - (int) x) != 0) {
                    printf("Error: the value should be 0 or 1\n");
                    printf("Enter your command:\n");
                    continue;
                }
                mark_errors((int)x);
                printf("Enter your command:\n");
                continue;
            } else if (strcmp(token, "print_board") == 0) {
                print_board();
            } else if (strcmp(token, "set") == 0) {
                xString = strtok(NULL, " \t\r\n");
                yString = strtok(NULL, " \t\r\n");
                zString = strtok(NULL, " \t\r\n");
                if (mode == 0) {
                    printf("ERROR: invalid command\n");
                    printf("Enter your command:\n");
                    continue;
                }
                if (xString == NULL || yString == NULL || zString == NULL) {
                    printf("ERROR: invalid command\n");
                    printf("Enter your command:\n");
                    continue;
                }
                if (strcmp(yString, "0") == 0 || strcmp(xString, "0") == 0) {
                    printf("Error: value not in range 0-%d\n", N);
                    printf("Enter your command:\n");
                    continue;
                }
                if (atoi(xString) == 0) {        /* x isn't an integer*/
                    printf("Error: value not in range 0-%d\n", N);
                    printf("Enter your command:\n");
                    continue;
                } else {
                    x = atof(xString);
                }
                if (atoi(yString) == 0) {
                    printf("Error: value not in range 0-%d\n", N);
                    printf("Enter your command:\n");
                    continue;
                } else {
                    y = atof(yString);
                }
                if (strcmp(zString, "0") != 0 && atoi(zString) == 0) {
                    printf("ERROR: invalid command\n");
                    printf("Enter your command:\n");
                    continue;
                } else {
                    z = atof(zString);
                }
                if (x <= 0 || y <= 0 || z < 0 || x > N || y > N || z > N || (x - (int) x) != 0 || (y - (int) y) != 0 ||
                (z - (int) z) != 0) { /*z can be 0*/
                    printf("Error: value not in range 0-%d\n", N);
                    printf("Enter your command:\n");
                    continue;
                }
                set((int)x, (int)y, (int)z);
            } else if (strcmp(token, "validate") == 0) {
                validateFlag = validate();
                if (validateFlag == 1) {
                    printf("Validation passed: board is solvable\n");
                } else if (validateFlag == 0) {
                    printf("Validation failed: board is unsolvable\n");
                }
            } else if (strcmp(token, "generate") == 0) {
                int numberOfEmptyCells;
                if (mode != 1) {
                    printf("ERROR: invalid command\n");
                    printf("Enter your command:\n");
                    continue;
                }
                numberOfEmptyCells = countNumberOfEmptyCells();
                xString = strtok(NULL, " \t\r\n");
                yString = strtok(NULL, " \t\r\n");
                if (xString == NULL || yString == NULL) {
                    printf("ERROR: invalid command\n");
                    printf("Enter your command:\n");
                    continue;
                } else {
                    x = atof(xString);
                }
                if (strcmp(yString, "0") != 0 && atoi(yString) == 0) {
                    printf("Error: value not in range 0-%d\n", numberOfEmptyCells);
                    printf("Enter your command:\n");
                    continue;
                } else {
                    y = atof(yString);
                }
                if (strcmp(xString, "0") != 0 && atoi(xString) == 0) {
                    printf("Error: value not in range 0-%d\n", numberOfEmptyCells);
                    printf("Enter your command:\n");
                    continue;
                }
                if (x < 0 || y < 0 || x > numberOfEmptyCells || y > numberOfEmptyCells || (x - (int) x) != 0 || (y - (int) y) != 0) {
                    if (numberOfEmptyCells != 0) {
                        printf("Error: value not in range 0-%d\n", numberOfEmptyCells);
                        printf("Enter your command:\n");
                        continue;
                    }
                }
                if (isBoardEmpty() == 0) { /* we must check that the board is completely empty*/
                    printf("Error: board is not empty\n");
                    printf("Enter your command:\n");
                    continue;
                }
                matrixBeforeGenerate = copyMatrix(matrix,N);
                generate((int)x, (int)y, 0);
                freeMatrix(matrixBeforeGenerate,N);
                printf("Enter your command:\n");
                continue;
            } else if (strcmp(token, "undo") == 0) {
                undo();
            } else if (strcmp(token, "redo") == 0) {
                redo();
            } else if (strcmp(token, "save") == 0) {
                xString = strtok(NULL, " \t\r\n");
                if (xString == NULL) {
                    printf("ERROR: invalid command\n");
                    printf("Enter your command:\n");
                    continue;
                }
                save(xString);
            } else if (strcmp(token, "num_solutions") == 0) {
                num_solutions();
                printf("Enter your command:\n");
                continue;

            } else if (strcmp(token, "autofill") == 0) {
                autofill();
            } else if (strcmp(token, "reset") == 0) {
                reset();
            } else if (strcmp(token, "exit") == 0) {
                freeMatrix(matrix, N);
                freeMatrix(fixedFlagMatrix, N);
                freeMatrix(errorsMatrix, N);
                freeMatrix(solMatrix, N);
                while (head != NULL || last != NULL) { /*clean the linkedList after successful gameover*/
                    deleteLast();
                }
                current = NULL;
                printf("Exiting...\n");
                exit(1);
            } else {
                printf("ERROR: invalid command\n");
            }
            printf("Enter your command:\n");
        }
    }
    /* this part is for an EOF exit. */
    freeMatrix(matrix, N);
    freeMatrix(fixedFlagMatrix, N);
    freeMatrix(errorsMatrix, N);
    freeMatrix(solMatrix, N);
    while (head != NULL || last != NULL) { /* clean the linkedList after successful gameover */
        deleteLast();
    }
    current = NULL;
    printf("Exiting...\n");
    exit(1);
}


/*Restart the game*/
void start_game() {
    n=3; /* default! */
    m=3;
    N = n*m;
    mode = 0; /* default! */
    markErrorsFlag = 1;
    matrix = create_empty_game_board();
    errorsMatrix = create_empty_game_board();
    fixedFlagMatrix = create_empty_game_board();
    solMatrix = create_empty_game_board();
    while (head != NULL || last != NULL) { /*clean the linkedList after a successful gameover*/
        deleteLast();
    }
    current = NULL;
    printf("Enter your command:\n");
    readAndReactBuffer(); /*start reading inputs*/

}
