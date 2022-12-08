#include "config.h"
#include "stack.h"

int main()
{
    $;
    Stack_t stk3 = {};
    stackCtor(&stk3, 100);
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
        



