#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "linkedList.h"
#include "mainAux.h"

node* current = NULL;
node* head = NULL;
node* last = NULL;

/* This method prints the undo-redo nodes from the beginning to the end */
void displayForward() {
    /*start from the beginning*/
    struct node *ptr = head;
    /*navigate till the end of the list*/
    printf("\n[ ");
    while(ptr != NULL) {
        printf("(%d,%d,%d) ",ptr->x,ptr->y,ptr->value);
        ptr = ptr->next;
    }
    printf(" ]\n");
    if (current == NULL) {
        printf("current=NULL ");
    } else {
        printf("current = (%d,%d,%d)\n ",current->x,current->y,current->value);
    }
    if (head == NULL) {
        printf("Head=NULL ");
    } else {
        printf("Head = (%d,%d,%d)\n ",head->x,head->y,head->value);
    }
    if (last == NULL) {
        printf("Last=NULL ");
    } else {
        printf("Last = (%d,%d,%d)\n ",last->x,last->y,last->value);
    }
}

/*insert a node at the very last location*/
void insertLast(int x, int y, int value, int ** matrixBeforeAutofill) {

    struct node *link = (struct node*) malloc(sizeof(struct node));
    link->x = x;
    link->y = y;
    link->value = value;
    link->next = NULL;
    link->prev = NULL;
    link->matrixBeforeAutofill = matrixBeforeAutofill;


    if(last != NULL) {
        last->next = link;
        link->prev = last;
    } else {
        head = link; /* that means that this is the first node to be on the linked list.*/
        current = link;
    }
    last = link;
    current = last;
}

/* Delete the tail in the redo-undo list after an insert operation was done */
int deleteAllTail(int currentFirstNullFlag) {
    struct node *ptrToDelete;
    struct node *ptr;
    struct node *toBeLast;
    if (head == NULL) {
        return currentFirstNullFlag;
    }
    if (current == NULL && currentFirstNullFlag == 1) {
        head = NULL;
        last = NULL;
        return currentFirstNullFlag;
    }
    if (current == NULL && currentFirstNullFlag == 0) {
        current = last;
        currentFirstNullFlag = -1;
        return currentFirstNullFlag;
    }
    ptr = current->next;
    toBeLast = current;

    while(ptr != NULL) {
        ptrToDelete = ptr->next;
        free(ptr);
        ptr = ptrToDelete;
    }
    last = toBeLast;
    current = last;
    return -1;
}

/* Delete the whole redo-undo list */
void deleteLast() {
    /*if only one link*/
    if(head->next == NULL) {
        if (head->matrixBeforeAutofill != NULL) {
            freeMatrix(head->matrixBeforeAutofill, N);
            head->matrixBeforeAutofill = NULL;
        }
        free(head);
        head = NULL;
    } else {
        if (last->prev->next != NULL) {
            if (last->prev->next->matrixBeforeAutofill != NULL) {
                freeMatrix(last->prev->next->matrixBeforeAutofill, N);
                last->prev->next->matrixBeforeAutofill = NULL;
            }
            free(last->prev->next);
            last->prev->next = NULL;
        }
    }
    last = last->prev;
}
