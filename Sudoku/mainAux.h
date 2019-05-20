#ifndef UNTITLED11_MAINAUX_H
#define UNTITLED11_MAINAUX_H
#include <stdio.h>
#include "stack.h"
#include "gurobi_c.h"

extern int ** matrix;
extern int ** fixedFlagMatrix;
extern int ** errorsMatrix;
extern int ** solMatrix;
extern int ** matrixBeforeGenerate;
extern int N,n,m,mode, markErrorsFlag, currentFirstNullFlag, numOfSolutions;

int ** create_empty_game_board();
void freeMatrix(int ** arr, int size);
int ** copyMatrix (int ** gameMatrix, int size) ;
void check_row(int ** gameMatrix, int N, int * options, int row);
void check_col(int ** gameMatrix, int N, int * options, int column);
void check_block(int ** gameMatrix, int * options, int n, int m, int topRow, int topCol);
int isFinished(int ** gameMatrix, int ** errorsMatrix, int size);
int isFixed (int i, int j);
void updateErrorsMatrix();
int * checkValidNumbers(int ** gameMatrix, int i, int j);
void loadGameBoard(FILE * fp);
void freeMatrixThreeD (int *** matrix3, int size);
int isErroneous();
int isBoardEmpty();
void updateSolMatrix(double * solVector);
void valUpd(int ** board, int *row, int *col, Stack *stk);
void idxPlus(int *row, int *col);
void idxMinus(int *row, int *col);
int exhaustiveBacktrack(int ** board);
void freeGurobiArrays (int * ind, double * val, char * vtype, double * solVector, GRBmodel * model, GRBenv * env);
int countNumberOfEmptyCells ();

#endif
