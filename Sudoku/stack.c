#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include "mainAux.h"

/* initalize stack data structure*/
int init(Stack *s){
    if(s == NULL){
        return 0;
    }
    s->size = 0;
    s->top = NULL;
    return 1;
}

/* handling pushing item into a stack */
int push(Stack *s, int val){
    StackNode *p;
    if(s == NULL){
        return -1;
    }
    p = (StackNode *)malloc(sizeof(StackNode));
    if(p == NULL){
        free(p);
        return -1;
    }
    p->val = val;
    p->next = s->top;
    s->top = p;
    s->size++;
    return 1;
}

/* handlin popping item out of a stack*/
int pop(Stack *s){
    StackNode *p;
    int val;
    if(s == NULL){
        return -1;
    }
    val = s->top->val;
    p = s->top;
    s->top = s->top->next;
    s->size--;
    free(p);
    return val;
}

/* return the value of the top item in the stack*/
int top(Stack *s){
    if(s == NULL){
        return -1;
    }
    return s->top->val;
}

/* pops out all the items from the stack and then frees the relevant pointer*/
void freeStack(Stack *s){
    if(s == NULL){
        return;
    }
    while(s->size > 0){
        pop(s);
    }
    free(s);
}
