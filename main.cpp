typedef double Elem_t;
typedef unsigned long long Canary_t;

#include "stack.h"

int main()
{
    $;
    Stack_t stk3 = {};
    stackCtor(&stk3, 9);
   for (int index = 0; index <= 70; ++index)
   {
       stackPush(&stk3, (double) index);
   }
   stackDump(&stk3, 0);
   for (size_t index = 0; index <= 60; ++index)
   {
       stackPop(&stk3);
   }
   stackDump(&stk3, 0);
}
        



