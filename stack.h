#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "LogLib.h"
#include <memory.h>

#ifndef DBGFILENAME
#define DBGFILENAME "debugFile.txt"
#endif

#define stackDump(stkptr, errors) stackDumpFunc(stkptr, errors, __LINE__, __func__, __FILE__, DBGFILEPTR);
#define stackCtor(stkptr, size) stackCtorFunc(stkptr, size, #stkptr + 1, __func__, __FILE__, __LINE__);

void print(char param);
void print(int param);
void print(const char* param);
void print(double param);

int getPoison(int elem);
float getPoison(float elem);
double getPoison(double elem);
char getPoison(char elem);
char* getPoison(char* elem);

const int _DOWN = 1;
const int _UP   = 0;

FILE* const dbgOpen(const char*);

FILE* const DBGFILEPTR = dbgOpen(DBGFILENAME);

struct DebugInf
{
    const char*  bornName; 
    const char*  bornFunction;
    const char*  bornFile;
    size_t bornLine;
};

struct Stack_t
{
    unsigned long long leftCanary= 0xDEADBEEF;
    size_t  size;
    size_t  capacity; 
    unsigned long long* leftDataCanary;
    Elem_t* data;
    unsigned long long* rightDataCanary;
    DebugInf debugInf;
    unsigned long long rightCanary = 0xDEDFADE;
};

enum Errors 
{
    noErrors             = 0,
    stkptrError          = 1 << 0,
    capacityError        = 1 << 1,
    dataError            = 1 << 2,
    sizeError            = 1 << 3,  
    sizeAndCapacityError = 1 << 4,
    recallocUpError      = 1 << 5,
    recallocDownError    = 1 << 6,
    leftCanaryError      = 1 << 7,
    rightCanaryError     = 1 << 8,
    leftDataCanaryError  = 1 << 9,
    rightDataCanaryError = 1 << 10,
};

void stackPoison(Stack_t*);

int stackCtorFunc(Stack_t* stk, size_t capacity, const char* stkName, const char* funcName, const char* fileName, const int line);

int stackDtor(Stack_t* stk);

int stackError(Stack_t* stk);

void stackDumpFunc(const Stack_t* stk, int errors, int line, const char* func, const char* file, FILE* dbgFile);

int stackPush(Stack_t* stk, Elem_t value);

Elem_t stackPop(Stack_t* stk, int* errors = nullptr);

Elem_t* stackRecalloc(Stack_t* stk, int param);

//-------------------------------------------------------

FILE* const dbgOpen(const char* dbgFileName)
{
    char buf[0] = {};
    FILE* fileptr = fopen(dbgFileName, "w");
    assert(fileptr != nullptr);
    setvbuf(fileptr, buf, _IONBF, 0);

    return fileptr;
}

int getPoison(int elem)
{
    return 0xBAD32DED;
}

float getPoison(float elem)
{
    return NAN;
}

char* getPoison(char* elem)
{
   return (char*) 13;
}

char getPoison(char elem)
{
    return '\0';
}

double getPoison(double elem)
{
    return NAN;
}

void print(char param)
{
    fprintf(DBGFILEPTR, "%c", param);
}

void print(int param)
{
    fprintf(DBGFILEPTR, "%d", param);
}

void print(const char* param)
{
    fprintf(DBGFILEPTR, "%s", param);
}

void print(double param)
{
    fprintf(DBGFILEPTR, "%lg", param);
}

//----------------------------------------------------------------

void stackPoison(Stack_t* stk)
{
    for (size_t index = 0; index < stk->capacity; ++index)
    {
        stk->data[index] = getPoison(stk->data[0]);
    }
}

int stackCtorFunc(Stack_t* stk, size_t capacity, const char* stkName, const char* funcName, const char* fileName, const int line)
{
    // ASSERTOK()
    assert(stk != nullptr);
    assert(capacity != 0);

    void* fullData             = calloc(1, capacity * sizeof(Elem_t) + 2 * sizeof(stk->leftDataCanary));
    stk->data                  = (Elem_t*) fullData + sizeof(stk->leftDataCanary);
    stk->leftDataCanary        = (unsigned long long*) fullData;
    stk->rightDataCanary       = (unsigned long long*) fullData + sizeof(stk->leftDataCanary) + sizeof(Elem_t) * capacity;
    *(stk->leftDataCanary)     = 0xCAFEBABE;
    *(stk->rightDataCanary)    = 0xCAFED00D;

    stk->capacity              = capacity;
    stk->size                  = 0;
    stk->debugInf.bornName     = stkName;
    stk->debugInf.bornFunction = funcName;
    stk->debugInf.bornFile     = fileName;
    stk->debugInf.bornLine     = line;


    stackPoison(stk);

    stackDump(stk, stackError(stk));

    return 0;
}

int stackDtor(Stack_t* stk)
{
    assert(stk != nullptr);
    stk->capacity                 = 0xDED32DED; // base
    stk->size                     = 0xDED32DED;
    *(stk->leftDataCanary)        = 0xDED32DED;
    *(stk->rightDataCanary)       = 0xDED32DED;
    *(stk->leftDataCanary)        = 0xDED32DED;
    *(stk->rightDataCanary)       = 0xDED32DED;
    stk->debugInf.bornName        = nullptr;
    stk->debugInf.bornFunction    = nullptr;
    stk->debugInf.bornFile        = nullptr;
    stk->debugInf.bornLine        = 0xDED32DED;
    free(stk->data);
    stk->data                     = nullptr;

    return 0;
}

int stackError(Stack_t* stk)
{
    assert(stk != nullptr);
    int errors = noErrors;
    if (stk)
    {
        if (isnan(stk->capacity) || stk->capacity < 0)
            errors |= capacityError;
        
        if (stk->size < 0)
            errors |= sizeError;

        if (!stk->data)
            errors |= dataError;

        if (stk->size > stk->capacity)
            errors |= sizeAndCapacityError;
        
        if (stk->leftCanary != 0xDEADBEEF)
            errors |= leftCanaryError;

        if (stk->rightCanary != 0xDEDFADE)
            errors |= rightCanaryError;

        if (*(stk->leftDataCanary)  != 0xCAFEBABE)
            errors |= leftDataCanaryError;

        if (*(stk->rightDataCanary) != 0xCAFED00D)
            errors |= rightDataCanaryError;
    }
    else
        errors |= stkptrError;

    return errors;
}

void stackDumpFunc(const Stack_t* stk, int errors, int line, const char* func, const char* file, FILE* dbgFile)
{
    fprintf(dbgFile, "%s at ",     func);
    fprintf(dbgFile, "%s",         file);
    fprintf(dbgFile, "(%d);\n",    line);
    fprintf(dbgFile, "stack[%p] ", stk);
    
    if (errors)
        fprintf(dbgFile, "(ERROR: %d)", errors);
    else
        fprintf(dbgFile, "(ok) ");
    fprintf(dbgFile, "Name = %s ", stk->debugInf.bornName); 
    fprintf(dbgFile, "at function %s at file %s(%d)\n", stk->debugInf.bornFunction, stk->debugInf.bornFile, stk->debugInf.bornLine);

    if (stk)
    {
        fprintf(dbgFile,     "{                 \n");
        fprintf(dbgFile,     "    size        = %lu\n", stk->size);
        fprintf(dbgFile,     "    capacity    = %lu\n", stk->capacity);
        fprintf(dbgFile,     "    leftCanary  = %p\n",  stk->leftCanary);
        fprintf(dbgFile,     "    rightCanary = %p\n\n",  stk->rightCanary);
        
        if (stk->data)
        {
            fprintf(dbgFile, "    fullData [%p]\n    {\n", stk->data);
            fprintf(dbgFile, "         leftDataCanary = %p\n", *(stk->leftDataCanary));
            for (size_t index = 0; index < stk->capacity; ++index)
            {
                if (stk->data[index] == getPoison(stk->data[0]) || isnan(stk->data[index]))
                {
                    fprintf(dbgFile, "         [%lu] = POISONED\n", index);
                }
                else
                {
                    fprintf(dbgFile, "        *[%lu] = ", index);
                    print(stk->data[index]);
                    fprintf(dbgFile, "\n");
                }
            }

                    fprintf(dbgFile, "         rightDataCanary = %p\n", *(stk->rightDataCanary));
                    fprintf(dbgFile, "    }\n");
        }
        else
        {
            $;
            fprintf(dbgFile, "fullData [%p] NULLPTR\n {", stk->data);
        }

            
        fprintf(dbgFile, "}\n");
    }
    else
        fprintf(dbgFile, "Wrong ptr on stack\n");

    fprintf(dbgFile, "----------------------------------------------------------\n");

}

int stackPush(Stack_t* stk, Elem_t value)
{
    int errors = noErrors;

    if (errors = stackError(stk))
    {
        stackDump(stk, errors);
    }
    else
    {
        (stk->data)[stk->size++] = value;
//        if (stk->size >= stk->capacity)
//        {
//            Elem_t* newptr = stackRecalloc(stk, _UP);
//
//            if (newptr)
//            {
//                stk->capacity *= 2;
//                free(stk->data);
//                stk->data = newptr;
//            }
//            else
//            {
//                errors |= recallocUpError;
//                return errors;         
//            }
//        }
    }
    
    return stackError(stk);
}

Elem_t stackPop(Stack_t* stk, int* errors)
{
    if (int _errors = stackError(stk))
    {
        stackDump(stk, _errors);

        if (errors) *errors = _errors;
    }
    else
    {
        stk->size--;
        Elem_t elem = stk->data[stk->size];
        stk->data[stk->size] =getPoison(elem);

        return elem;
    }

    return stk->data[0];
}

//Elem_t* stackRecalloc(Stack_t* stk, int param)
//{
//    if (param)
//    {
//        return (Elem_t*) recalloc(stk->data, stk->capacity/2, sizeof(stk->data[0]));
//    }
//    
//    return (Elem_t*) recalloc(stk->data, stk->capacity*2, sizeof(stk->data[0]));
//}

