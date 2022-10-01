typedef int Elem_t;
typedef unsigned long long Canary_t;

#include "stack.h"

int main()
{
    $;
    Stack_t stk3 = {};
    stackCtor(&stk3, 9);
    stackPush(&stk3, 228);
    for (size_t index = 0; index <= 70; ++index)
    {
        stackPush(&stk3, index);
    }
    for (size_t index = 0; index <= 60; ++index)
    {
        stackPop(&stk3);
    }
    *((char*) stk3.data - sizeof(Canary_t)) = 0x0000000;
    stk3.data[0] = -2;
    stackPop(&stk3);
}
        



