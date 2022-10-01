typedef int Elem_t;
typedef unsigned long long Canary_t;

#include "stack.h"

int initStackAndTest(Stack_t *a)
{
    stackCtor(a, 0);

    return 0;
}


int main()
{
    $;
    Stack_t stk3 = {};
    stackCtor(&stk3, 10);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackDump(&stk3, 0);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackDump(&stk3, 0);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackDump(&stk3, 0);
    stackDump(&stk3, stackPush(&stk3, 7));
    stackPop(&stk3);
    stackDump(&stk3, 0);
    stackPop(&stk3);
    stackPop(&stk3);
    stackDump(&stk3, 0);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
    stackPop(&stk3);
}


