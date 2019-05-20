#include "commands.h"
#include "mainAux.h"
#include "linkedList.h"
#include "gurobi_c.h"
#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


/* handles solve mode, freeing all used boards, load a board to be solved */
void solve(char * fileName){
    FILE * fp;
    mode = 2; /*we move to solve mode when this command is executed*/
    while (head != NULL || last != NULL) { /*clean the linkedList before inserting a new board to the game*/
        deleteLast();
    }
    current = NULL;
    freeMatrix(errorsMatrix,N); /*we must delete the old one with the old N value*/
    freeMatrix(fixedFlagMatrix,N);
    freeMatrix(solMatrix,N);
    fp = fopen(fileName, "r");
    if (fp == NULL){
        printf("Error: File doesn't exist or cannot be opened\n");
    }
    freeMatrix(matrix,N);
    loadGameBoard(fp);
    rewind(fp);
    errorsMatrix = create_empty_game_board();
    updateErrorsMatrix();
    solMatrix = create_empty_game_board();
    print_board();
    fclose(fp);
}

/* handles EDIT mode, freeing all used boards, load board to be edited or creates a new one*/ 
void edit(char * fileName) {
    mode = 1;
    while (head != NULL || last != NULL) { /*clean the linkedList before inserting a new board to the game*/
        deleteLast();
    }
    current = NULL;
    freeMatrix(errorsMatrix,N); /*we must delete the old one with the old N value*/
    freeMatrix(fixedFlagMatrix,N);
    freeMatrix(solMatrix,N);
    freeMatrix(matrix,N);
    if (fileName == NULL){ /* no paramter was given */
        m = n = 3;
        N = m * n;
        matrix = create_empty_game_board();
        fixedFlagMatrix = create_empty_game_board();
        errorsMatrix = create_empty_game_board();
        solMatrix = create_empty_game_board();
        print_board();
    } else {
        FILE * fp;
        fp = fopen(fileName, "r");
        if (fp == NULL && mode == 1){ /*mode: edit*/
            printf("Error: File cannot be opened\n");
        }
        loadGameBoard(fp);
        rewind(fp);
        errorsMatrix = create_empty_game_board();
        updateErrorsMatrix();
        solMatrix = create_empty_game_board();
        print_board();
        fclose(fp);
    }
}

/*the method updates the markErrorsFlag option. available only in Solve mode*/
void mark_errors(int x) {
    if (mode != 2) {
        printf("ERROR: invalid command\n");
        return;
    }
    if (x != 0 && x != 1) {
        printf("Error: the value should be 0 or 1\n");
        return;
    }
    markErrorsFlag = x;
    return;
}


/*This method prints the game board in the specified format*/
void print_board() {
    int i, j, k;
    if (mode == 0) { /*this method is available only in edit mode and solve mode"*/
        printf("ERROR: invalid command\n");
        return;
    }
    for (i = 0; i < N; i++) {
        if (i % m == 0) {
            for (k = 0; k < 4 * N + m + 1; k++) {
                printf("-");
            }
            printf("\n");
        }
        for (j = 0; j < N; j++) {
            if (j % n == 0 && j != 0) {
                printf("|");
            } else if (j == 0) {
                printf("|");
            }
            if (matrix[i][j] == 0) {
                printf("    ");
            } else {
                if (mode == 2) {
                    if(isFixed(i, j) == 1) {
                        printf(" %2d.", matrix[i][j]);
                    }  else if (errorsMatrix[i][j] == 1 && markErrorsFlag == 1) {
                        printf(" %2d*", matrix[i][j]);
                    } else {
                        printf(" %2d ", matrix[i][j]);
                    }
                } else if (mode == 1  || markErrorsFlag == 1) { /*mode 1 = edit mode*/
                    if (errorsMatrix[i][j] == 1) {
                        printf(" %2d*", matrix[i][j]);
                    } else {
                        printf(" %2d ", matrix[i][j]);
                    }
                }
            }
        }
        printf("|\n");
    }
    for (k = 0; k < 4 * N + m + 1; k++) {
        printf("-");
    }
    printf("\n");
}

/* sets a value to a specified cell in the board */
int set(int y, int x, int z) {
    int finishedFlag, lastValue;
    int *valid;
    if (mode == 0) { /*this method is available only in edit mode and solve mode"*/
        printf("ERROR: invalid command\n");
        return -1;
    }
    if (isFixed(x - 1, y - 1) == 1 && mode != 1) {
        printf("Error: cell is fixed\n");
        return -1;
    }
    else if (matrix[x-1][y-1] == z){
        print_board();
        return 0;
    }
    else {
        if (z == 0) {
            lastValue = matrix[x - 1][y - 1];
            matrix[x - 1][y - 1] = 0;
            currentFirstNullFlag = deleteAllTail(currentFirstNullFlag);
            insertLast(x,y,lastValue,NULL);
        } else {     /* z != 0 ==> check validity of number*/
            valid = checkValidNumbers(matrix, x - 1, y - 1);
            free(valid);
        }
        lastValue = matrix[x - 1][y - 1];
        matrix[x - 1][y - 1] = z;
        updateErrorsMatrix();
        currentFirstNullFlag = deleteAllTail(currentFirstNullFlag);
        insertLast(x,y,lastValue,NULL);
        print_board();
        finishedFlag = isFinished(matrix, errorsMatrix, N);


        if (finishedFlag == 1) { /*game finished*/
            printf("Puzzle solved successfully\n");
            mode = 0; /*move to Init mode*/
            freeMatrix(matrix, N);
            freeMatrix(fixedFlagMatrix, N);
            freeMatrix(errorsMatrix, N);
            freeMatrix(solMatrix, N);
            start_game();
            return 1;
        } else if (finishedFlag == 2) {
            mode = 2;
            printf("Puzzle solution erroneous\n");
            return 1;
        }
        return 0;
    }
}

/* going back one move from last move done by the user */
void undo () {
    int currentValue;
    if (mode == 0) {
        printf("ERROR: invalid command\n");
        return;
    }
    if (head == NULL || last == NULL) { /*the linked list is empty = no moves to undo.*/
        printf("Error: no moves to undo\n");
        return;
    }
    if (current == NULL && currentFirstNullFlag == 1) { /*currentFirstNullFlag=1 represents that current is left to the most left node*/
        printf("Error: no moves to undo\n");  /*we made all the redo steps, no more left*/
        return;
    }
    if (current == NULL && currentFirstNullFlag == 0) { /*currentFirstNullFlag=0 represents that current is right to the most right node*/
        current = last; /*we are "after" the last node, we can start making redos from last*/
        currentFirstNullFlag = -1; /*currentFirstNullFlag=-1 represents that current isnt NULL and holds a node on the list*/
    }
    if (current != NULL && current->matrixBeforeAutofill != NULL) { /*undo for a autofill command*/
        int ** matrixBeforeUndo;
        int i,j;
        matrixBeforeUndo = copyMatrix(matrix,N); /*saves the values that were on the board*/
        freeMatrix(matrix,N);
        matrix = current->matrixBeforeAutofill; /*make the change on the board*/
        updateErrorsMatrix();
        print_board();
        for (i = 0; i < N ; i++) {
            for (j = 0; j < N; j++) {
                if (matrixBeforeUndo[i][j] != matrix[i][j]) { /*the <i,j> cell had been changed and we should print a message about it*/

                    if (matrixBeforeUndo[i][j] == 0 && matrix[i][j] != 0) {
                        printf("Undo %d,%d: from _ to %d\n",j+1,i+1,matrix[i][j]);
                    } else if (matrixBeforeUndo[i][j] != 0 && matrix[i][j] == 0) {
                        printf("Undo %d,%d: from %d to _\n",j+1,i+1,matrixBeforeUndo[i][j]);
                    } else {
                        printf("Undo %d,%d: from %d to %d\n",j+1,i+1,matrixBeforeUndo[i][j],matrix[i][j]);
                    }

                }
            }
        }

        current->matrixBeforeAutofill = matrixBeforeUndo; /*save it for a future redo*/
        if (current->prev == NULL) {
            currentFirstNullFlag = 1;
        }
        current = current->prev; /*in undo we first take the action and then take the pointer one step backward*/
        return;
    }
    if(current != NULL) {
        currentValue = matrix[current->x - 1][current->y - 1]; /*save the value that was on the board*/
        matrix[current->x -1][current->y -1] = current->value; /*make the change on the board*/
        updateErrorsMatrix();
        print_board();
        if (currentValue == 0 && current->value == 0) {
            printf("Undo %d,%d: from _ to _\n",current->y,current->x);
        } else if (currentValue == 0) {
            printf("Undo %d,%d: from _ to %d\n",current->y,current->x,current->value);
        } else if (current->value == 0) {
            printf("Undo %d,%d: from %d to _\n",current->y,current->x,currentValue);
        } else {
            printf("Undo %d,%d: from %d to %d\n",current->y,current->x,currentValue,current->value);
        }
        current->value = currentValue; /*save it for a future redo*/
        if (current->prev == NULL) {
            currentFirstNullFlag = 1;
        }
        current = current->prev; /*in undo we first take the action and then take the pointer one step backward*/
    }
}

/* going forward one move from last move done by the user*/
void redo () {
    int currentValue;
    if (mode == 0) {
        printf("ERROR: invalid command\n");
        return;
    }
    if (current != NULL) { /*in redo we first take the pointer forwars and then take the action*/
        if (current->next == NULL) {
            currentFirstNullFlag = 0;
        }
        current = current->next;
    }
    if (head == NULL || last == NULL) { /*the linked list is empty = no moves to redo.*/
        printf("Error: no moves to redo\n");
        return;
    }
    if (current == NULL && currentFirstNullFlag == 0) { /*we made all the redo steps, no more left*/
        printf("Error: no moves to redo\n");
        return;
    }
    if (current == NULL && currentFirstNullFlag == 1) {  /*we are "before" the first node, we can start making redos from head*/
        current = head;
        currentFirstNullFlag = -1;
    }
    if (current != NULL && current->matrixBeforeAutofill != NULL) { /*redo for a autofill command*/

        int ** matrixBeforeRedo;
        int i,j;
        matrixBeforeRedo = copyMatrix(matrix,N); /*saves the values that were on the board*/
        freeMatrix(matrix,N);
        matrix = current->matrixBeforeAutofill; /*make the change on the board*/
        updateErrorsMatrix();
        print_board();
        for (i = 0; i < N ; i++) {
            for (j = 0; j < N; j++) {
                if (matrixBeforeRedo[i][j] != matrix[i][j]) { /*the <i,j> cell had been changed and we should print a message about it*/

                    if (matrixBeforeRedo[i][j] == 0 && matrix[i][j] != 0) {
                        printf("Redo %d,%d: from _ to %d\n",j+1,i+1,matrix[i][j]);
                    } else if (matrixBeforeRedo[i][j] != 0 && matrix[i][j] == 0) {
                        printf("Redo %d,%d: from %d to _\n",j+1,i+1,matrixBeforeRedo[i][j]);
                    } else {
                        printf("Redo %d,%d: from %d to %d\n",j+1,i+1,matrixBeforeRedo[i][j],matrix[i][j]);
                    }

                }
            }
        }
        current->matrixBeforeAutofill = matrixBeforeRedo; /*save it for a future redo*/
        if (current->next == NULL) {
            currentFirstNullFlag = 0;
        }
        return;
    }
    if (current != NULL) {
        currentValue = matrix[current->x - 1][current->y - 1]; /*save the value that was on the board*/
        matrix[current->x -1][current->y -1] = current->value; /*make the change on the board*/
        updateErrorsMatrix();
        print_board();
        if (currentValue == 0 && current->value == 0) {
            printf("Redo %d,%d: from _ to _\n",current->y,current->x);
        } else if (currentValue == 0) {
            printf("Redo %d,%d: from _ to %d\n",current->y,current->x,current->value);
        } else if (current->value == 0) {
            printf("Redo %d,%d: from %d to _\n",current->y,current->x,currentValue);
        } else {
            printf("Redo %d,%d: from %d to %d\n",current->y,current->x,currentValue,current->value);
        }
        current->value = currentValue; /*save it for a future undo*/
        if (current->next == NULL) {
            currentFirstNullFlag = 0;
        }
    }
}

/* fills cells with only one legal value left to put in them */
void autofill () {
    int i,j,finishedFlag, changedFlag;
    int *** statusMatrix;
    int ** matrixBeforeAutofill;
    int * validNow; /* will be responsible not causing an erroneous value when using autofill*/
    matrixBeforeAutofill = copyMatrix(matrix,N);
    changedFlag = 0;
    if (mode != 2) {
        printf("ERROR: invalid command\n");
        freeMatrix(matrixBeforeAutofill,N);
        return;
    }
    if (isErroneous() == 1) {
        printf("Error: board contains erroneous values\n");
        freeMatrix(matrixBeforeAutofill,N);
        return;
    }

    statusMatrix = (int ***) malloc(N * sizeof(int **));
    for (i = 0; i < N; i++) {
        statusMatrix[i] = (int **) malloc(N * sizeof(int *));
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            statusMatrix[i][j] = checkValidNumbers(matrix,i,j);
        }
    }

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            validNow = checkValidNumbers(matrix,i,j);
            if (matrix[i][j] == 0 && statusMatrix[i][j][0] == 1 && validNow[0] != 0) {
                matrix[i][j] = statusMatrix[i][j][1];
                changedFlag = 1;
                printf("Cell <%d,%d> set to %d\n",j+1,i+1,statusMatrix[i][j][1]);
            }
            free(validNow);
            validNow = NULL;
        }
    }
    freeMatrixThreeD(statusMatrix,N);
    /*changedFlag means that there was a change in matrix and we should insert matrixBeforeAutofill to the linked list for a future undo/redo */
    if (changedFlag == 1) {
        insertLast(-1,-1,-1,copyMatrix(matrixBeforeAutofill,N));
    }
    print_board();
    finishedFlag = isFinished(matrix, errorsMatrix, N);
    if (finishedFlag == 1) { /*game finished*/
        printf("Puzzle solved successfully\n");
        freeMatrix(matrixBeforeAutofill,N);
        mode = 0; /*move to Init mode*/
        freeMatrix(matrix,N);
        freeMatrix(errorsMatrix,N);
        freeMatrix(fixedFlagMatrix,N);
        freeMatrix(solMatrix,N);
        start_game();
        return;
    }
        freeMatrix(matrixBeforeAutofill,N);
        return;
}

/* returns the the game to its original state*/
void reset () {
    if (mode == 0) {
        printf("ERROR: invalid command\n");
        return;
    }
    if (current == NULL && currentFirstNullFlag == 0) {
        current = last;
    }
    while (current != NULL) {
        if (current->x != -1) { /*we must check that this is a regular undo and not a autofill full board undo*/
            matrix[current->x -1][current->y -1] = current->value; /*make the change on the board*/
        } else {
                freeMatrix(matrix,N);
                matrix = copyMatrix(current->matrixBeforeAutofill,N);
        }
        current = current->prev; /*in undo we first take the action and then take the pointer one step backward*/
    }
    while (head != NULL || last != NULL) {
        deleteLast();
    }
    free(current);
    current = NULL;
    currentFirstNullFlag = -1;
    freeMatrix(errorsMatrix,N);
    errorsMatrix = create_empty_game_board();
    updateErrorsMatrix();
    printf("Board reset\n");
    return;
}

/* creating a board by randomly filling given number of cells with random legal values */
void generate(int x, int y, int iteration) {
    int randCounter,col,row,indexOfChosen,i,j;
    int * options;
    randCounter = 0;
    if (mode != 1) {
        printf("ERROR: invalid command\n");
        return;
    }
    if (iteration == 1000) {
        printf("Error: puzzle generator failed\n");
        return;
    }
    if (isBoardEmpty() == 0) { /* we must check that the board is completely empty*/
        printf("Error: board is not empty\n");
        return;
    }
    while (randCounter < x) { /*we choose x different cells*/
        col = rand() % N;
        row = rand() % N;
        if (matrix[row][col] == 0) { /*it is a different cell*/
            randCounter++;
            options = checkValidNumbers(matrix, row, col); /*we generate all legal values possible for the chosen cell*/
            if (options[0] == 0) { /*we failed to create the board because no legal option available for a specific cell*/
                free(options);
                freeMatrix(matrix,N);
                matrix = create_empty_game_board();
                freeMatrix(fixedFlagMatrix, N);
                freeMatrix(errorsMatrix, N);
                freeMatrix(solMatrix,N);
                solMatrix = create_empty_game_board();
                fixedFlagMatrix = create_empty_game_board();
                errorsMatrix = create_empty_game_board();
                while (head != NULL || last != NULL) { /*clean the linkedList after successful gameover*/
                    deleteLast();
                }
                current = NULL;
                generate(x,y,iteration + 1);
                return;
            } else {
                indexOfChosen = rand() % options[0] + 1;
                matrix[row][col] = options[indexOfChosen]; /*we randomize a legal value and take it*/
                free(options);
            }
        }
    }

    randCounter = 0;
    /* we have to mark the fixed cells, otherwise, validate will delete the matrix values! */
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (matrix[i][j] != 0) {
                fixedFlagMatrix[i][j] = 1;
            }
        }
    }
        if (validate() == 0) { /*we failed to create the board because ILP didnt find a possible solution*/
            freeMatrix(matrix,N);
            matrix = create_empty_game_board();
            freeMatrix(fixedFlagMatrix, N);
            freeMatrix(errorsMatrix, N);
            freeMatrix(solMatrix,N);
            fixedFlagMatrix = create_empty_game_board();
            errorsMatrix = create_empty_game_board();
            solMatrix = create_empty_game_board();
            while (head != NULL || last != NULL) { /*clean the linkedList after successful gameover*/
                deleteLast();
            }
            current = NULL;
            generate(x,y,iteration + 1);
            return;
       } else {
            /*validate was successful and we now have the matrix in solMatrix*/
            freeMatrix(matrix,N);
            matrix = copyMatrix(solMatrix,N);
        }
    while (randCounter < N*N - y) { /*we choose N*N - y different cells to be cleared*/
        col = rand() % N;
        row = rand() % N;
        if (matrix[row][col] != 0) { /* it is a different cell */
            randCounter++;
            matrix[row][col] = 0; /* we clear this cell */
        }
    }
    /* if we got here then we successfully built a game board and we will now mark the fixed cells and print the board*/
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (matrix[i][j] != 0) {
                fixedFlagMatrix[i][j] = 1;
            }
        }
    }
    insertLast(-1,-1,-1,copyMatrix(matrixBeforeGenerate,N));
    print_board();
    return;
}

/* saves the current board to a file, if succeed return 1, else 0 */
int save(char * fileName){
    FILE * fp;
    int i, j;
    if (mode == 0){ /* init mode */
        printf("ERROR: invalid command\n");
        return 0;
    }
    else{
        if (mode == 1){ /* edit mode */
            if (isErroneous() == 1){
                printf("Error: board contains erroneous values\n");
                return 0;
            }
            if (validate() == 0){
                printf("Error: board validation failed\n");
                return 0;
            }
            fp = fopen(fileName, "w");
            if (fp == NULL){
                printf("Error: File cannot be created of modified\n");
                return 0;
            }
            fprintf(fp, "%d %d\n", m, n);
            for (i = 0; i < N; i++){
                for(j = 0; j < N; j++){
                    if (matrix[i][j] == 0){
                        fprintf(fp, "%d", 0);
                    }
                    else{
                        fprintf(fp, "%d.", matrix[i][j]);
                    }
                    if (j < N-1){
                        fprintf(fp, " ");
                    }
                }
                fprintf(fp, "\r\n");
            }
        }
        else{
            fp = fopen(fileName, "w");
            if (fp == NULL){
                printf("Error: File cannot be created of modified\n");
                return 0;
            }
            fprintf(fp, "%d %d\n",m,n);
            for (i = 0; i < N; i++){
                for(j = 0; j < N; j++){
                    if (matrix[i][j] == 0){
                        fprintf(fp, "%d", 0);
                    }
                    else{
                        if (fixedFlagMatrix[i][j] == 1){
                            fprintf(fp, "%d.", matrix[i][j]);
                        }
                        else{
                            fprintf(fp, "%d", matrix[i][j]);
                        }
                    }
                    if (j < N-1){
                        fprintf(fp, " ");
                    }
                }
                fprintf(fp, "\r\n");
            }
        }
        printf("Saved to: %s\n",fileName);
        fclose(fp);
        return 1;
    }
}

/* give a hint to the user by showing the solution of a single cell x,y */
void hint (int x, int y) {
    if (mode != 2) {
        printf("ERROR: invalid command\n");
        return;
    }
    if (isErroneous() == 1) {
        printf("Error: board contains erroneous values\n");
        return;
    }
    if (isFixed(x-1,y-1) == 1) {
        printf("Error: cell is fixed\n");
        return;
    }
    if (isFixed(x-1,y-1) == 0 && matrix[x-1][y-1] != 0) {
        printf("Error: cell already contains a value\n");
        return;
    }
    if (validate() == 0) {
        printf("Error: board is unsolvable\n");
        return;
    } else {
        printf("Hint: set cell to %d\n",solMatrix[x-1][y-1]);
    }
    return;
}

/* adds the needed constraints to the relevant GUROBI model */
int addGurobiConstraints(int * ind, double * val, GRBmodel * model, GRBenv * env,char * vtype, double * solVector) {
    int i,j,k,error,iblock,jblock, count;
    int fixedInd[1];
    double fixedVal[1];
    /* (1) First constraint: the requirement that each 1<val<N must appear exactly once per row */
    for(i = 0; i < N; i++){
        for (k = 0; k < N; k++) {
            for (j = 0; j < N; j++) {
                ind[j] = i*N*N + j*N + k;
                val[j] = 1.0;
            }
            error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
            if (error) {
                printf("ERROR %d GRBaddconstr1(): %s\n", error, GRBgeterrormsg(env));
                freeGurobiArrays (ind,val,vtype,solVector,model,env);
                return 0;
            }
        }
    }
    /* (2) Second constraint: the requirement that each 1<val<N must appear exactly once per column */
    for (k = 0; k < N; k++) {
        for (j = 0; j < N; j++) {
            for(i = 0; i < N; i++){
                ind[i] = i*N*N + j*N + k;
                val[i] = 1.0;
            }
            error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
            if (error) {
                printf("ERROR %d GRBaddconstr2(): %s\n", error, GRBgeterrormsg(env));
                freeGurobiArrays (ind,val,vtype,solVector,model,env);
                return 0;
            }
        }
    }
    /* (3) Third constraint: the requirement that each 1<val<N must appear exactly once per block */
    for(k = 0; k < N; k++){
        for (iblock = 0; iblock < N; iblock=iblock+m){
            for (jblock = 0; jblock < N; jblock=jblock+n){
                count = 0;
                for (i = iblock; i < iblock+m; i++){
                    for (j = jblock; j < jblock+n; j++){
                        ind[count] = i*N*N + j*N + k;
                        val[count] = 1.0;
                        count++;
                    }
                }
                error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
                if (error) {
                    printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
                    freeGurobiArrays (ind,val,vtype,solVector,model,env);
                    return 0;
                }
            }
        }
    }
    /* (4) Fourth constraint: a cell can contain only one legal value */
    for(i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            for (k = 0; k < N; k++) {
                ind[k] = i * N * N + j * N + k;
                val[k] = 1.0;
            }
            error = GRBaddconstr(model, N, ind, val, GRB_EQUAL, 1.0, NULL);
            if (error) {
                printf("ERROR %d GRBaddconstr4(): %s\n", error, GRBgeterrormsg(env));
                freeGurobiArrays (ind,val,vtype,solVector,model,env);
                return 0;
            }
        }
    }
    /*(5) Fifth constraint: we dont want to change fixed values so we will add that to the constraints */
    for(i = 0; i < N; i++){
        for (j = 0; j < N; j++) {
            for (k = 0; k < N; k++) {
                if (matrix[i][j] != 0 && k + 1 == matrix[i][j]) { /*we have a value in the cell and we dont want ILP to change it.*/
                    fixedInd[0] = i * N * N + j * N + k;
                    fixedVal[0] = 1.0;
                    error = GRBaddconstr(model, 1, fixedInd, fixedVal, GRB_EQUAL, 1.0, NULL);
                    if (error) {
                        printf("ERROR %d GRBaddconstr5(): %s\n", error, GRBgeterrormsg(env));
                        freeGurobiArrays (ind,val,vtype,solVector,model,env);
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

/* validates the current board using the Gurobi for ILP and ensuring it is solvable */
int validate() {
    GRBenv *env = NULL;
    GRBmodel *model = NULL;
    int * ind;
    double * val;
    char * vtype;
    int optimstatus;
    double objval;
    double * solVector;
    int i,error;
    if (mode == 0) {
        printf("ERROR: invalid command\n");
        return -1;
    }
    if (isErroneous() == 1) {
        printf("Error: board contains erroneous values\n");
        return -1;
    }
    ind = (int *)calloc(N, sizeof(int));
    val = (double *)calloc(N, sizeof(double));
    vtype = (char *)calloc(N*N*N, sizeof(char));
    solVector = (double *)calloc(N*N*N, sizeof(double));
    if (ind == NULL || val == NULL || vtype == NULL || solVector == NULL) {
        printf("calloc failed");
        return -1;
    }
	/* defining N^3 variables as follows: xijk is an integer binary variable, 1 meaning the value at cell i,j is equal to k, 0 otherwise */
    for (i = 0; i < N*N*N; i++){
        vtype[i] = GRB_BINARY;
    }
    error = GRBloadenv(&env, "sudoku.log");     /* Create environment - log file is sudoku.log */
    if (error) {
        printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
        freeGurobiArrays (ind,val,vtype,solVector,model,env);
        return 0;
    }
    GRBsetintparam(env,GRB_INT_PAR_LOGTOCONSOLE,0); /*cancels gurobi prints*/
    error = GRBnewmodel(env, &model, "Sudoku", N*N*N, NULL, NULL, NULL, vtype, NULL);/* Create a model named "Sudoku"*/
    if (error) {
        printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
        freeGurobiArrays (ind,val,vtype,solVector,model,env);
        return 0;
    }
    if (addGurobiConstraints(ind,val,model,env,vtype,solVector) == 0) {
        return 0;
    }
    error = GRBoptimize(model);     /* Optimize model */
    if (error) {
        /*printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));*/
        freeGurobiArrays (ind,val,vtype,solVector,model,env);
        return 0;
    }
    error = GRBwrite(model, "sudoku.lp");     /* Write model to 'sudoku.lp' */
    if (error) {
        /*printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));*/
        freeGurobiArrays (ind,val,vtype,solVector,model,env);
        return 0;
    }
    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);     /* Capture solution information */
    if (error) {
        /*printf("ERROR %d GRBgetintattr1(): %s\n", error, GRBgeterrormsg(env));*/
        freeGurobiArrays (ind,val,vtype,solVector,model,env);
        return 0;
    }
    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
    if (error) {
        /*printf("ERROR %d GRBgetintattr2(): %s\n", error, GRBgeterrormsg(env));*/
        /*printf("optimStatus: %d\n",optimstatus);*/
        freeGurobiArrays (ind,val,vtype,solVector,model,env);
        return 0;
    }
    error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0,N*N*N, solVector);
    if (error) {
        /*printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));*/
        freeGurobiArrays (ind,val,vtype,solVector,model,env);
        return 0;
    }
    updateSolMatrix(solVector);
    if (optimstatus == GRB_OPTIMAL) {
        /* status is ok */
    } else if (optimstatus == GRB_INF_OR_UNBD) {
        /* Model is infeasible or unbounded */
        freeGurobiArrays (ind,val,vtype,solVector,model,env);
        return 0;
    }
    else {
        printf("Optimization was stopped early\n");
        freeGurobiArrays (ind,val,vtype,solVector,model,env);
        return -1;
    }
    freeGurobiArrays (ind,val,vtype,solVector,model,env); /* Free dynamic assigned arrays */
    return 1;
}

/* prints the number of solutions to the current board, using backtrack */
void num_solutions () {
    int ** copiedMatrix;
    numOfSolutions = 0;
    if (mode == 0) {
        printf("ERROR: invalid command\n");
        return;
    }
    if (isErroneous() == 1) {
        printf("Error: board contains erroneous values\n");
        return;
    }
    copiedMatrix = copyMatrix(matrix,N);
    numOfSolutions = exhaustiveBacktrack(copiedMatrix);
    freeMatrix(copiedMatrix,N);
    printf("Number of solutions:%d\n",numOfSolutions);
    /*in this project we were asked not to take care of the case when numOfSolutions == 0*/
    if (numOfSolutions == 1) {
        printf("This is a good board!\n");
    } else if (numOfSolutions > 1) {
        printf("The puzzle has more than 1 solution, try to edit it further\n");
    }
    return;
}
