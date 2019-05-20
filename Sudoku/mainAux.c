#include <stdio.h>
#include <stdlib.h>
#include "linkedList.h"
#include "stack.h"
#include <assert.h>
#include "commands.h"
#include <string.h>
#include "gurobi_c.h"

int currentFirstNullFlag = -1;
int ** matrix;
int ** fixedFlagMatrix;
int ** errorsMatrix;
int ** solMatrix;
int ** matrixBeforeGenerate;
int N,n,m,mode, markErrorsFlag, numOfSolutions;

/* This method creates an empty matrix with size NXN */
int ** create_empty_game_board() {
    int ** game_board;
    int i;
    game_board = (int **)calloc(N, sizeof(int *));
    for (i = 0; i < N; i++){
        game_board[i] = (int *)calloc(N, sizeof(int));
    }
    return game_board;
}

/*This method frees a matrix from the memory*/
void freeMatrix(int ** arr, int size){
    int i;
    if (arr != NULL) {
        for (i = 0; i < size; i++) {
            if (arr != NULL || arr[i] != NULL) {
                free(arr[i]);
                arr[i] = NULL;
            }
        }
        free(arr);
        arr = NULL;
    }
}

/* This method copies an NxN matrix. notice that it copies the matrix and not just the pointer */
int ** copyMatrix (int ** gameMatrix, int size) {
    int ** gameMatrix3;
    int i,j;
    gameMatrix3 = create_empty_game_board();
    for(i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            gameMatrix3[i][j] = gameMatrix[i][j];
        }
    }
    return gameMatrix3;
}


/* This method puts 1 in options[i-1] if i is in row */
void check_row(int ** gameMatrix, int N, int * options, int row){
    int k;
    for (k = 0; k < N; k++){
        if (gameMatrix[row][k] != 0){
            options[gameMatrix[row][k]-1] = 1;
        }
    }
}
/* This method puts 1 in options[i-1] if i is in column */
void check_col(int ** gameMatrix, int N, int * options, int column){
    int k;
    for (k = 0; k < N; k++){
        if (gameMatrix[k][column] != 0){
            options[gameMatrix[k][column]-1] = 1;
        }
    }
}

/* This method puts 1 in options[i-1] if i is in the box that its left upwards cell */
void check_block(int ** gameMatrix, int * options, int n, int m, int topRow, int topCol){
    int k;
    int l;
    for (k=0; k<m; k++){
        for(l=0; l<n; l++){
            if (gameMatrix[topRow+k][topCol+l] != 0){
                options[gameMatrix[topRow+k][topCol+l]-1] = 1;
            }
        }
    }
}

/*return 1 if and only if the whole board filled with numbers!= 0 and all values on board are legal.
 * that means that the game is finished */
int isFinished(int ** gameMatrix, int ** errorsMatrix, int size){
    int i;
    int j;
    for (i = 0; i < size; i++){
        for (j = 0; j < size; j++){
            if (gameMatrix[i][j] == 0){
                return 0;
            }
        }
    }
    for (i = 0; i < size; i++){
        for (j = 0; j < size; j++){
            if (errorsMatrix[i][j] == 1){
                return 2; /*means the board is all filled but the solution is erroneous*/
            }
        }
    }
    return 1;
}

/* This method checks id the cell (i,j) is fixed and returns 1 if it is fixed */
int isFixed (int i, int j) {
    return fixedFlagMatrix[i][j];
}

/* This method goes over the game board and updates errorsMatrix accordingly */
void updateErrorsMatrix() {
    int i, j,row,col,topRow,topCol,k,l,value;
    freeMatrix(errorsMatrix, N);
    errorsMatrix = create_empty_game_board();
    for (row = 0; row < N; row++) {
        for (col = 0; col < N; col++) {
            value = matrix[row][col];
            if (value == 0) {
                continue;
            }
            for (j = 0; j < N && j != col; j++) {
                if (matrix[row][j] == value) {
                    errorsMatrix[row][col] = 1;
                    errorsMatrix[row][j] = 1;
                }
            }
            for (i = 0; i < N && i != row; i++) {
                if (matrix[i][col] == value) {
                    errorsMatrix[row][col] = 1;
                    errorsMatrix[i][col] = 1;
                }
            }
            topRow = row - row%m;
            topCol = col - col%n;
            for (k=0; k<m; k++){
                for(l=0; l<n; l++){
                    if (topRow+k == row && topCol+l == col) {
                        continue;
                    }
                    if (matrix[topRow+k][topCol+l] == value){
                        errorsMatrix[row][col] = 1;
                        errorsMatrix[topRow+k][topCol+l] = 1;
                    }
                }
            }
        }
    }
}


/*This method returns an array that contains it size in first value, and then all valid values legal for <i,j> cell*/
int * checkValidNumbers(int ** gameMatrix, int i, int j){
    int * options;
    int * res;
    int cnt,k,l,index;
    index = 1;
    cnt = 0;
    options = (int *)calloc(N, sizeof(int));
    if (options == NULL) {
        printf("calloc failed");
        return options;
    }
    check_row(gameMatrix, N, options, i);
    check_col(gameMatrix, N, options, j);
    check_block(gameMatrix, options,n,m, i - i%m, j - j%n); /* we give it the top left corner of the block*/
    for (l = 0; l < N; l++) {
        if (options[l] == 1){
            cnt++;
        }
    }
    if (cnt == N) {
        res = (int *)calloc(1, sizeof(int)+1);
        if(res != NULL) {
            res[0] = 0;
        } else {
            printf("Error: checkValidNumbers has failed\n");
            free(res);
            exit(1);
        }
    } else {
        res = (int *)calloc(N-cnt+1, sizeof(int));
        if(res != NULL) {
            res[0] = N-cnt;
            for (k = 0; k < N; k++){
                if (options[k] == 0){
                    res[index] = k+1;
                    index++;
                }
            }
        } else {
            printf("Error: checkValidNumbers has failed\n");
            free(res);
            exit(1);
        }
    }
    free(options);
    return res;
}

/*load and parse a txt file into a game board*/
void loadGameBoard(FILE * fp) {
    int i, j;
    char * value;
    char line[1024];
    i = j = 0;
    if (fgets(line, 1024, fp) != NULL) {
        m = atoi(strtok(line, " \t\r\n"));
        n = atoi(strtok(NULL, " \t\r\n"));
        N = n * m;
    }
    matrix = create_empty_game_board();
    fixedFlagMatrix = create_empty_game_board();
    while (fgets(line, 1024, fp) != NULL) {
        value = strtok(line, " \t\r");
        while (value != NULL) {
            if (strcmp(value, "\n") != 0) {
                matrix[i][j] = atoi(value);
                if (value[strlen(value)-1] == '.' || value[strlen(value)-2] == '.') {
                    fixedFlagMatrix[i][j] = 1;
                }
                value = strtok(NULL, " \t\r");
                j++;
            } else {
                if (j == N && i < N - 1) {
                    i++;
                    j = 0;
                }
                break;
            }
            if (j == N && i < N - 1) {
                i++;
                j = 0;
            }
        }
    }
    return;
}

/*frees from memory 3d matrixes and their pointer*/
void freeMatrixThreeD (int *** matrix3, int size) {
    int i,j;
    for(i = 0;i < size;i++){
        for(j = 0;j < size;j++){
            free(matrix3[i][j]);
        }
        free(matrix3[i]);
    }
    free(matrix3);
}


/*returns 1 if game board contains erroneous values and otherwise return 1*/
int isErroneous() {
    int i,j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (errorsMatrix[i][j] == 1) {
                return 1;
            }
        }
    }
    return 0;
}


/*returns 1 if game board is completely empty an otherwise return 0*/
int isBoardEmpty() {
    int i,j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (matrix[i][j] != 0) { /* first, we must check that the board is completely empty*/
                return 0;
            }
        }
    }
    return 1;
}

/* This method takes the answer from the gurobi (an array) and moves it into a matrix */
void updateSolMatrix(double * solVector){
    int cnt, i, j;
    freeMatrix(solMatrix,N);
    solMatrix = create_empty_game_board();
    for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
            for (cnt = 0; cnt < N; cnt++){
                if (solVector[i*N*N+j*N+cnt] == 1.0){
                    solMatrix[i][j] = cnt+1;
                }
            }
        }
    }
}

/*This method returns 1 if num is in row, else 0*/
int check_row2(int ** gameMatrix, int size, int row, int col, int num){
    int k;
    for (k = 0; k < size; k++){
        if (k == col){
            continue;
        }
        if (gameMatrix[row][k] == num){
            return 1;
        }
    }
    return 0;
}

/*This method returns 1 if num is in column, else 0*/
int check_col2(int ** arr, int size, int row, int column, int num){
    int k;
    for (k = 0; k < size; k++){
        if (k == row){
            continue;
        }
        if (arr[k][column] == num){
            return 1;
        }
    }
    return 0;
}

/*This method returns 1 if num is in the box that its left upwards cell, else 0*/
int check_box2(int ** arr, int topRow, int topCol, int rowi, int colj, int num){
    int row;
    int col;
    for (row = 0; row < m; row++){
        for (col = 0; col < n; col++){
            if (topRow+row == rowi && topCol+col == colj){
                continue;
            }
            if (arr[topRow+row][topCol+col] == num){
                return 1;
            }
        }
    }
    return 0;
}

/* checks if the assignment of num in the (i,j) position on board is legal */
int isValid(int ** gameMatrix, int size, int i, int j, int num){
    if ((check_row2(gameMatrix, size, i, j, num)) == 0 && (check_col2(gameMatrix, size, i, j, num)) == 0  &&
        (check_box2(gameMatrix, i - i%m, j - j%n, i, j, num)) == 0  ) {
        return 1;
    }
    return 0;
}

/* Helps the backtrack algorithm to increase its index*/
void idxPlus(int *row, int *col){
    if(*col < N-1){
        *col = *col + 1;
    }
    else{
        *row = *row + 1;
        *col=0;
    }
}

/* Helps the backtrack algorithm to increase its index*/
void idxMinus(int *row, int *col){
    if(*col > 0){
        *col = *col - 1;
    }
    else{
        *row = *row - 1;
        *col=N-1;
    }
}

void valUpd(int ** board, int *row, int *col, Stack *s){
    board[*row][*col]++;
    while (isValid(board,N,*row,*col,board[*row][*col])==0 && board[*row][*col]<=N){
        board[*row][*col]++;
    }
    if(board[*row][*col] <= N){ /*checking why the upper while loop ended : valid or no valid value*/
        idxPlus(row,col);
    }
    else{ /* no valid values left */
        pop(s);
        board[*row][*col] = 0;
        idxMinus(row,col);
    }
}

/* Implements the exhaustiveBacktrack algorithm with the stack in order to count the number of solutions*/
int exhaustiveBacktrack(int ** board){
    int counter = 0, row = 0, col = 0, flag; /*flag = 1 means that value was 0 before*/
    Stack *s;
    s = (Stack *)malloc(sizeof(Stack));
    init(s);
    while(!(row == -1 && col == N-1)){
        if(row == N){
            counter++;
            row--; /*we go back to the right bottom corner*/
            col = N-1;
            continue;
        }
        if(s->size > row*N + col){ /*going back*/
            flag = top(s);
            if(flag){
                valUpd(board,&row, &col, s);
            }
            else { /* flag == 0 means that the cell contained a legal value before */
                pop(s);
                idxMinus(&row,&col);
            }
        }
        else{ /*going forward*/
            if(board[row][col] != 0){
                push(s,0);
                idxPlus(&row,&col);
            }
            else {
                push(s,1);
                valUpd(board, &row, &col,s);
            }
        }
    }
    freeStack(s);
    return counter;
}

/* Frees the enviroment, the model and the arrays used by the Gurobi */
void freeGurobiArrays (int * ind, double * val, char * vtype, double * solVector, GRBmodel * model, GRBenv * env) {
    free(ind);
    free(val);
    free(vtype);
    free(solVector);
    GRBfreemodel(model); /* Free model */
    GRBfreeenv(env); /* Free environment */

}

/* This method counts the number of empty cells on board and used by the generate method */
int countNumberOfEmptyCells () {
    int i,j,counter;
    counter = 0;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (matrix[i][j] == 0) {
                counter++;
            }
        }
    }
    return counter;
}
