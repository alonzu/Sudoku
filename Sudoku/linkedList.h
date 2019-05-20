#ifndef UNTITLED11_LINKEDLIST_H
#define UNTITLED11_LINKEDLIST_H

typedef struct node {
    int x;
    int y;
    int value;
    int ** matrixBeforeAutofill;

    struct node *next;
    struct node *prev;
} node;
extern struct node* current;
extern struct node* head;
extern struct node* last;
void displayForward();
void insertLast(int x, int y, int value, int ** matrixBeforeAutofill);
int deleteAllTail(int currentFirstNullFlag);
void deleteLast();

#endif
