#include <stdio.h>
#include "pico/stdlib.h"
#include <assert.h>

typedef struct node_header node;

struct node_header
{
    void *data;
    node *next;
};

typedef struct stack_header stack;

struct stack_header
{
    node *top;
};

// CLIENT INTERFACE START

typedef stack *stack_t;
typedef void *stack_elem;
typedef void elem_free_fn(stack_elem e);

bool stack_empty(stack_t S);
stack_t stack_new();
void push(stack_t S, stack_elem elem);
stack_elem pop(stack_t S);

// CLIENT INTEFACE END