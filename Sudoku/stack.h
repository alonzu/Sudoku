#ifndef UNTITLED4_STACK_H
#define UNTITLED4_STACK_H

#include <stdlib.h>
typedef struct stackNode{
    int val;
    struct stackNode *next;
}StackNode;

typedef struct stack{
    int size;
    StackNode *top;
}Stack;

int init(Stack *stk);
int push(Stack *stk,int val);
int pop(Stack *stk);
int top(Stack *stk);
void freeStack(Stack *stk);

#endif
