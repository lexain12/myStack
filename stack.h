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

enum StackStatus
{
    statusAlive = 0,
    statusDead  = 1,
};

struct DebugInf
{
    const char*  bornName; 
    const char*  bornFunction;
    const char*  bornFile;
    size_t bornLine;
    StackStatus stackStatus;
    size_t dataHash;
    size_t structHash;
};

struct Stack_t
{
    unsigned long long leftCanary= 0xDEADBEEF;
    size_t  size;
    size_t  capacity; 
    unsigned long long* leftDataCanary;
    char* fullData;
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
    stackResizeError     = 1 << 11,
    memAllocError        = 1 << 12,
    dataHashError        = 1 << 13,
    structHashError      = 1 << 14,
};

const char* ErrorMessages[] = {
    "No errors :)",
    "Wrong ptr on structure with stack",
    "Wrong capacity",
    "No pointer on data (stack with elements)",
    "Size bigger than capacity => problem with stack size",
    "Can't realloc up",
    "Can't realloc down",
    "Left canary died :( RIP",
    "Oh no, we lost right canary",
    "leftDataCanary is damaged",
    "rightDataCanary is damaged", 
    "dataHash has changed",
    "structHash has changed",
};

void arrayPoison(Elem_t*, size_t);

int stackCtorFunc(Stack_t* stk, size_t capacity, const char* stkName, const char* funcName, const char* fileName, const int line);

int stackDtor(Stack_t* stk);

int stackError(Stack_t* stk);

void stackDumpFunc(const Stack_t* stk, int errors, int line, const char* func, const char* file, FILE* dbgFile);

int stackPush(Stack_t* stk, Elem_t value);

Elem_t stackPop(Stack_t* stk, int* errors = nullptr);

int stackResize(Stack_t* stk, int param);

void errorPrint(int errors);

size_t countHash(char* key, size_t len);

int checkHash(Stack_t* stk);
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

void arrayPoison(Elem_t* data, size_t limiter)
{
    for (size_t index = 0; index < limiter; ++index)
    {
        data[index] = getPoison(data[0]);
    }
}

int stackCtorFunc(Stack_t* stk, size_t capacity, const char* stkName, const char* funcName, const char* fileName, const int line)
{
    int errors = 0;
    
    if (stk == nullptr) 
        return errors |= stkptrError;
   
    capacity                   = (capacity == 0 ? 0 : (capacity / 10 + 1) * 10);
    stk->capacity              = capacity;
    stk->size                  = 0;
    stackDump(stk, 0);

    if (capacity != 0)  
    {
        stk->fullData          = (char*) calloc(1, capacity * sizeof(Elem_t) + 2 * sizeof(unsigned long long));

        if (stk->fullData == nullptr)
            return errors |= memAllocError;

        stk->data              = (Elem_t*) (stk->fullData + sizeof(unsigned long long));
        stk->leftDataCanary    = (unsigned long long*) stk->fullData;
        stk->rightDataCanary   = (unsigned long long*) (stk->fullData + sizeof(unsigned long long) + sizeof(Elem_t) * capacity);

    }

    *(stk->leftDataCanary)     = 0xCAFEBABE;
    *(stk->rightDataCanary)    = 0xCAFED00D;
    stk->debugInf.bornName     = stkName;
    stk->debugInf.bornFunction = funcName;
    stk->debugInf.bornFile     = fileName;
    stk->debugInf.bornLine     = line;
    stk->debugInf.stackStatus  = statusAlive;

    stackDump(stk, 0);

    arrayPoison(stk->data, stk->capacity);
    size_t structHash          = countHash((char*) stk, sizeof(Stack_t));
    size_t dataHash            = countHash((char*) stk->data, sizeof(Elem_t) * stk->capacity);
    fprintf(DBGFILEPTR, "adsfadsfasdf %d", structHash);
    stk->debugInf.structHash   = structHash;
    stk->debugInf.dataHash     = dataHash;

    stackDump(stk, errors |= stackError(stk));

    return errors;
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
    free(stk->fullData);
    stk->debugInf.stackStatus     = statusDead;
    stk->data                     = nullptr;
    stk->fullData                 = nullptr;
    free(stk);

    return 0;
}

int stackError(Stack_t* stk)
{
    assert(stk != nullptr);
    size_t epsiloh = -400;

    int errors = noErrors;
    if (stk)
    {
        if (isnan(stk->capacity) || stk->capacity > epsiloh)
            errors |= capacityError;
        
        if (stk->size > epsiloh)
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
        
        errors |= checkHash(stk);
    }
    else
        errors |= stkptrError;

    return errors;
}

void stackDumpFunc(const Stack_t* stk, int errors, int line, const char* func, const char* file, FILE* dbgFile)
{
    fprintf(dbgFile, "-----------------stackDump----------------\n");
    fprintf(dbgFile, "%s at ",     func);
    fprintf(dbgFile, "%s",         file);
    fprintf(dbgFile, "(%d);\n",    line);
    fprintf(dbgFile, "stack[%p] ", stk);
    if (stk->debugInf.stackStatus)
        fprintf(dbgFile, "status: Dead ");
    else
        fprintf(dbgFile, "status: Alive ");
    
    if (errors)
    {
        fprintf(dbgFile, "(ERROR: %d)\n", errors);
        errorPrint(errors);
    }
    else
        fprintf(dbgFile, "(no errors) ");
    fprintf(dbgFile, "Name = %s ", stk->debugInf.bornName); 
    fprintf(dbgFile, "at function %s at file %s(%lu)\n", stk->debugInf.bornFunction, stk->debugInf.bornFile, stk->debugInf.bornLine);

    if (stk)
    {
        fprintf(dbgFile,     "{                 \n");
        fprintf(dbgFile,     "    size        = %lu\n", stk->size);
        fprintf(dbgFile,     "    capacity    = %lu\n", stk->capacity);
        fprintf(dbgFile,     "    leftCanary  = %p\n",  stk->leftCanary);
        fprintf(dbgFile,     "    rightCanary = %p\n",  stk->rightCanary);
        fprintf(dbgFile,     "    structHash  = %lu\n", stk->debugInf.structHash);
        fprintf(dbgFile,     "    dataHash    = %lu\n",   stk->debugInf.dataHash);
        
        if (stk->data)
        {
            fprintf(dbgFile, "    fullData [%p] data[%p]\n    {\n", stk->fullData, stk->data);
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

    fprintf(dbgFile, "-----------stackDump-End------------------\n");

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
        if (stk->size >= stk->capacity)
        {
            stackResize(stk, _UP);
        }
        (stk->data)[stk->size++] = value;
    }
    
    stk->debugInf.dataHash = countHash((char*) stk->data, sizeof(Elem_t) * stk->capacity);
    
    return errors |= stackError(stk);
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
        if (stk->size <= stk->capacity / 2 - (stk->capacity / 4) && stk->capacity > 10)
        {
            _errors |= stackResize(stk, _DOWN);
            stk->capacity /= 2;
        }

        stk->size--;
        Elem_t elem = stk->data[stk->size];
        stk->data[stk->size] =getPoison(elem);

        if (errors) *errors |= _errors;

        return elem;
    }

    stk->debugInf.dataHash = countHash((char*) stk->data, sizeof(Elem_t) * stk->capacity);

    return stk->data[0];
}

int stackResize(Stack_t* stk, int param)
{
    int errors = 0;
    stackDump(stk, errors = stackError(stk));

    if (param)
        stk->capacity /= 2;
    else
        stk->capacity *= 2;

    if (char* newData = (char*) realloc(stk->fullData, sizeof(stk->data[0]) * stk->capacity + sizeof(unsigned long long) * 2))
        stk->fullData = newData;
    else
        errors |= stackResizeError;

    stk->data                  = (Elem_t*) (stk->fullData + sizeof(unsigned long long));
    arrayPoison(stk->data + stk->size, stk->capacity - stk->size);

    stk->leftDataCanary        = (unsigned long long*) stk->fullData;
    stk->rightDataCanary       = (unsigned long long*) (stk->fullData + sizeof(unsigned long long) + sizeof(Elem_t) * stk->capacity);
    *(stk->leftDataCanary)     = 0xCAFEBABE;
    *(stk->rightDataCanary)    = 0xCAFED00D;
    stackDump(stk, errors |= stackError(stk));

    return errors;
}

void errorPrint(int errors)
{
    int curError = 1;

    size_t index = 0;

    fprintf(DBGFILEPTR, "-----------------Errors-------------------\n");
    while (curError <= errors)
    {
        if (curError & errors)
        {
            fprintf(DBGFILEPTR, "    Error[%d] - %s\n", curError, ErrorMessages[index]);
        }
        curError <<= 1;
        index += 1;
    }
    fprintf(DBGFILEPTR, "-------------End-of-errors----------------\n");
}

size_t countHash(char* key, size_t len)
{
    size_t hash = noErrors;

    for (size_t i = 0; i < len; ++i) 
        hash = 33*hash + key[i];

    return hash;
}

int checkHash(Stack_t* stk)
{
    if (stk == nullptr) return stkptrError;

    int errors = 0;

    size_t oldStructHash = stk->debugInf.structHash;
    size_t oldDataHash   = stk->debugInf.dataHash;

    stk->debugInf.structHash = 0;
    stk->debugInf.dataHash   = 0;
    
    if (countHash((char*) stk, sizeof(Stack_t)) != oldStructHash)
        errors |= structHashError;
    if (countHash((char*) stk->data, sizeof(Elem_t) * stk->capacity) != oldDataHash)
        errors |= dataHashError;

    stk->debugInf.structHash = oldStructHash;
    stk->debugInf.dataHash   = oldDataHash;

    return errors;
}
