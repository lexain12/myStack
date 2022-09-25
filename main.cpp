typedef int Elem_t;

#include "stack.h"

int main()
{
    $;
    Stack_t stk3 = {};
    stackCtor(&stk3, 10);

    stackDump(&stk3, 0);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackPush(&stk3, 7);
    stackDump(&stk3, 0);
}


