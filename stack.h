#include <ctype.h>
#include <assert.h>
#include <math.h>
#include "LogLib.h"
#include <memory.h>

#define CANARYGUARD 1
#define HASHGUARD 1

#ifndef DBGFILENAME
#define DBGFILENAME "debugFile.txt"
#endif

#define stackDump(stkptr, errors) stackDumpFunc(stkptr, errors, __LINE__, __func__, __FILE__, DBGFILEPTR);
#define stackCtor(stkptr, size)   stackCtorFunc(stkptr, size, #stkptr, __func__, __FILE__, __LINE__);

void print(char param);
void print(int param);
void print(const char* param);
void print(double param);

// Elem_t Poison
int getPoison(int elem);
float getPoison(float elem);
double getPoison(double elem);
char getPoison(char elem);
char* getPoison(char* elem);

const int DefaultCapacity       = 10;

const int DestructionValue      = 0xDED32DED;
#if CANARYGUARD
const Canary_t LeftCanary       = 0xDEADBEEF;
const Canary_t RightCanary      = 0xDEDFADE;
const Canary_t LeftDataCanary   = 0xCAFEBABE;
const Canary_t RightDataCanary  = 0xCAFED00D;
#endif

enum Mode 
{
    DOWN = 1,
    UP   = 2,
};

FILE* const dbgOpen(const char* filename); // trash

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
#if HASHGUARD
    size_t dataHash;
    size_t structHash;
#endif
};

struct Stack_t
{
#if CANARYGUARD
    Canary_t leftCanary = LeftCanary;
#endif
    size_t  size;
    size_t  capacity; 

    Elem_t* data;

    DebugInf debugInf;
#if CANARYGUARD
    Canary_t rightCanary = RightCanary;
#endif
};

enum Errors 
{
    noErrors             = 0,
    stkptrError          = 1 << 0,
    capacityError        = 1 << 1,
    dataError            = 1 << 2,
    sizeError            = 1 << 3,  
    sizeAndCapacityError = 1 << 4,
    stackResizeError     = 1 << 5,
    memAllocError        = 1 << 6,
    rightCanaryError     = 1 << 8,
    leftCanaryError      = 1 << 7,
    leftDataCanaryError  = 1 << 9,
    rightDataCanaryError = 1 << 10,
    dataHashError        = 1 << 11,
    structHashError      = 1 << 12,
};

const char* ErrorMessages[] = { // make it better
    "No errors :)",
    "Wrong ptr on structure with stack",
    "Wrong capacity",
    "No pointer on data (stack with elements)",
    "Size bigger than capacity => problem with stack size",
    "Can't resize the stack",
    "Can't allocate memory",
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

void stackDumpFunc(Stack_t* stk, int errors, int line, const char* func, const char* file, FILE* dbgFile);

int stackPush(Stack_t* stk, Elem_t value);

Elem_t stackPop(Stack_t* stk, int* errors = nullptr);

int stackResize(Stack_t* stk, Mode mode);

void errorPrint(int errors);

size_t countHash(char* key, size_t len);

int checkHash(Stack_t* stk);

Canary_t* getLeftDataCanary(Stack_t* stk);

Canary_t* getRightDataCanary(Stack_t* stk);

void countHashes(Stack_t* stk);
//-------------------------------------------------------

FILE* const dbgOpen(const char* dbgFileName)
{
    char buf[0] = {};
    FILE* fileptr = fopen(dbgFileName, "w");
    if (fileptr != nullptr)
    {
        setvbuf(fileptr, buf, _IONBF, 0);
    }

    return fileptr;
}

int getPoison(int elem)
{
    return DestructionValue;
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

void arrayPoison(Elem_t* data, size_t limiter) // size??? const???
{
    for (size_t index = 0; index < limiter; ++index)
    {
        data[index] = getPoison(data[0]);
    }
}

int stackCtorFunc(Stack_t* stk, size_t capacity, const char* stkName, const char* funcName, const char* fileName, const int line)
{
    int errors      = 0;
    size_t dataHash = 0;
    
    if (stk == nullptr) 
        return errors |= stkptrError;
   
    capacity                   = (capacity == 0 ? 0 : (capacity / DefaultCapacity + 1) * DefaultCapacity);
    stk->capacity              = capacity;
    stk->size                  = 0;

    if (capacity != 0)  
    {
#if CANARYGUARD
        char* newData = (char*) calloc(1, capacity * sizeof(Elem_t) + 2 * sizeof(Canary_t));
#else
        char* newData = (char*) calloc(1, capacity * sizeof(Elem_t);
#endif

        if (!newData)
            return errors |= memAllocError;

#if CANARYGUARD 
        stk->data                  = (Elem_t*) (newData + sizeof(Canary_t));
        *(getLeftDataCanary(stk))  = LeftDataCanary;
        *(getRightDataCanary(stk)) = RightDataCanary;
#else 
        stk->data                  = (Elem_t*) newData;
#endif
        arrayPoison(stk->data, stk->capacity);
#if HASHGUARD
        dataHash                = countHash((char*) stk->data, sizeof(Elem_t) * stk->capacity);
#endif
    }
    $;

    stk->debugInf.bornName     = stkName;
    stk->debugInf.bornFunction = funcName;
    stk->debugInf.bornFile     = fileName;
    stk->debugInf.bornLine     = line;
    stk->debugInf.stackStatus  = statusAlive;
#if HASHGUARD 
    size_t structHash          = countHash((char*) stk, sizeof(Stack_t));
    stk->debugInf.structHash   = structHash;

    if (dataHash) 
        stk->debugInf.dataHash = dataHash;
    else
        stk->debugInf.dataHash = 0;
#endif
    stackDump(stk, errors |= stackError(stk));

    return errors;
}

int stackDtor(Stack_t* stk)
{
    if (int errors = stackError(stk))
    {
        stackDump(stk, errors);
        return errors;
    }

    stk->capacity                 = DestructionValue; // base
    stk->size                     = DestructionValue;
#if CANARYGUARD
    *(getRightDataCanary(stk))    = DestructionValue;
    *(getLeftDataCanary(stk))     = DestructionValue;
    stk->leftCanary               = DestructionValue;
    stk->rightCanary              = DestructionValue;
#endif
    stk->debugInf.stackStatus     = statusDead;
    stk->data                     = nullptr;
    stk->debugInf.structHash      = NULL;
    stk->debugInf.dataHash        = NULL;    
    free(getLeftDataCanary(stk));
    stk                           = nullptr;
    
    return 0;
}

int stackError(Stack_t* stk)
{
    size_t epsiloh = (size_t) -1 / 2;

    int errors = noErrors;
    if (stk)
    {
        if (stk->capacity > epsiloh)
            errors |= capacityError;
        
        if (stk->size > epsiloh)
            errors |= sizeError;

        if (stk->size > stk->capacity)
            errors |= sizeAndCapacityError;
#if CANARYGUARD 
        if (stk->leftCanary != LeftCanary)
            errors |= leftCanaryError;

        if (stk->rightCanary != RightCanary)
            errors |= rightCanaryError;
#endif
        if (!stk->data)
            return errors |= dataError;
#if CANARYGUARD
        if (*(getLeftDataCanary(stk))  != LeftDataCanary)
            errors |= leftDataCanaryError;

        if (*(getRightDataCanary(stk)) != RightDataCanary)
            errors |= rightDataCanaryError;
#endif

        errors |= checkHash(stk);
        
    }
    else
        errors |= stkptrError;

    return errors;
}

void stackDumpFunc(Stack_t* stk, int errors, int line, const char* func, const char* file, FILE* dbgFile)
{
    $;
    fprintf(dbgFile, "-----------------stackDump----------------\n");
    fprintf(dbgFile, "%s at ",     func);
    fprintf(dbgFile, "%s",         file);
    fprintf(dbgFile, "(%d);\n",    line);
    fprintf(dbgFile, "stack[%p] ", stk);
    if (stk->debugInf.stackStatus)
        fprintf(dbgFile, "status: Dead ");
    else
        fprintf(dbgFile, "status: Alive ");
    
    $;
    if (errors)
    {
        fprintf(dbgFile, "(ERROR: %d)\n", errors);
        fprintf(dbgFile, "-----------------Errors-------------------\n");
    #define ErrorPrint(error, text)                                       \
                if (error & errors)                                       \
                {                                                         \
                    fprintf(DBGFILEPTR, "ERROR ["#error"] " #text "\n"); \
                }
        ErrorPrint(noErrors,             No errors);
        ErrorPrint(stkptrError,          Wrong ptr on structure with stack);
        ErrorPrint(capacityError,        Wrong capacity);
        ErrorPrint(dataError,            No pointer on data (stack with elements));
        ErrorPrint(sizeError,            Bad Size of stack);
        ErrorPrint(sizeAndCapacityError, Size bigger than capacity => problem with stack size)
        ErrorPrint(stackResizeError,     Cannot resize the stack);
        ErrorPrint(memAllocError,        Cannot allocate memory);
#if CANARYGUARD
        ErrorPrint(leftCanaryError,      Left canary died (RIP));
        ErrorPrint(rightCanaryError,     Oh no we lost right canary);
        ErrorPrint(leftDataCanaryError,  leftDataCanary is damaged);
        ErrorPrint(rightDataCanaryError, rightDataCanary is damaged);
#endif
#if HASHGUARD
        ErrorPrint(dataHashError,        dataHash has changed);
        ErrorPrint(structHashError,      structHash has changed);
#endif
        fprintf(DBGFILEPTR, "-------------End-of-errors----------------\n");
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
#if CANARYGUARD
        fprintf(dbgFile,     "    leftCanary  = %p\n",  stk->leftCanary);
        fprintf(dbgFile,     "    rightCanary = %p\n",  stk->rightCanary);
#endif
#if HASHGUARD
        fprintf(dbgFile,     "    structHash  = %lu\n", stk->debugInf.structHash);
#endif
        
        if (stk->data)
        {
#if HASHGUARD
            fprintf(dbgFile,     "    dataHash    = %lu\n",   stk->debugInf.dataHash);
#endif
            fprintf(dbgFile, "    data[%p]\n    {\n", stk->data);
#if CANARYGUARD
            fprintf(dbgFile, "         leftDataCanary = %p\n", *(getLeftDataCanary(stk)));
#endif
            for (size_t index = 0; index < stk->capacity; ++index)
            {
                if (stk->data[index] == getPoison(stk->data[0]) || isnan(stk->data[index]))
                {
                    fprintf(dbgFile, "         [%lu] = POISONED\n", index);
                }
                else {
                    fprintf(dbgFile, "        *[%lu] = ", index);
                    print(stk->data[index]);
                    fprintf(dbgFile, "\n");
                }
            }
#if CANARYGUARD
                    fprintf(dbgFile, "         rightDataCanary = %p\n", *(getRightDataCanary(stk)));
#endif
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
        return errors;
    }

    stackResize(stk, UP);
    (stk->data)[stk->size++] = value;
#if HASHGUARD 
    countHashes(stk);
#endif
    
    return errors |= stackError(stk);
}

Elem_t stackPop(Stack_t* stk, int* errors)
{
    if (int errors_ = stackError(stk)) // loh
    {
        stackDump(stk, errors_);
        if (errors) *errors = errors_;
    }
    else
    {

        stackResize(stk, DOWN);
        stk->size--; // --stk->size
        Elem_t elem = stk->data[stk->size];
        stk->data[stk->size] = getPoison(elem);

        if (errors) *errors |= errors_;
#if HASHGUARD
        countHashes(stk);
#endif

        return elem;
    }
#if HASHGUARD
    countHashes(stk);
#endif

    return stk->data[0];
}

//int stackResize(Stack_t* stk, Mote isDown)
//{
//    int errors         = 0;
//    size_t newCapacity = 0;
//
//    if (stk->capacity == 0)
//    {
//        char* newData = (char*) calloc(1, DefaultCapacity * sizeof(Elem_t) + 2 * sizeof(Canary_t));
//        if (!newData) return errors |= dataError;
//
//        stk->data = (Elem_t*) ((char*) newData + sizeof(Canary_t));
//        stk->capacity = DefaultCapacity;
//    }
//    else
//    {
//        if (isDown)
//        {
//            if (stk->size <= stk->capacity / 2 - (stk->capacity / 4) && stk->capacity > DefaultCapacity)
//            {
//                newCapacity /= 2;
//            }
//
//        }
//        else        newCapacity *= 2;
//
//        if (char* newData = (char*) realloc(getLeftDataCanary(stk), sizeof(stk->data[0]) * newCapacity + sizeof(Canary_t) * 2))
//        {
//            stk->data     = (Elem_t*) (newData + sizeof(Canary_t));
//            stk->capacity = newCapacity;
//            arrayPoison(stk->data + stk->size, stk->capacity - stk->size);
//        }
//        else
//            return errors |= stackResizeError;
//    }
//
//    *(getLeftDataCanary(stk))     = LeftDataCanary;
//    *(getRightDataCanary(stk))    = RightDataCanary;
//
//    stk->debugInf.dataHash = countHash((char*) stk->data, sizeof(Elem_t) * stk->capacity);
//
//    stackDump(stk, errors |= stackError(stk));
//
//    return errors;
//}
int stackResize(Stack_t* stk, Mode mode)
 {
     if (stk == NULL) return NULL;
 
     int errors = stackError(stk);
 
     size_t newCapacity = 0;
 
     if (mode == UP)
     {
         if (stk->capacity != 0 && stk->size < stk->capacity)
         {
             return errors;
         }
 
         newCapacity  = (stk->capacity == 0) ? DefaultCapacity : stk->capacity * 2;
     }
     else if (mode == DOWN)
     {
         if (stk->capacity > DefaultCapacity && stk->size <= stk->capacity * 3 / 8)
         {
             newCapacity = stk->capacity / 2;
         }
         else
             return errors;
     }
     else
     {
         stackDump(stk, errors |= stackError(stk))
         
         return errors;
     }
 
     char* newData = nullptr;
 
     if (stk->capacity == 0 && mode == UP)
     {
#if CANARYGUARD
         newData = (char*) calloc(1, 2 * sizeof(Canary_t) + newCapacity * sizeof(Elem_t));
#else
         newData = (char*) calloc(1, newCapacity * sizeof(Elem_t));
#endif
     }
     else if (stk->capacity == 0 && mode == DOWN)
     {
         errors |= stackResizeError;
         stackDump(stk, errors |= stackError(stk));
 
         return errors;
     }
     else
     {
#if CANARYGUARD
         newData = (char*) realloc(getLeftDataCanary(stk), 2 * sizeof(Canary_t) + newCapacity * sizeof(Elem_t));
#else
         newData = (char*) realloc(stk->data, newCapacity * sizeof(Elem_t));
#endif
     }
 
     if (newData == NULL)
     {
         errors |= memAllocError;
         stackDump(stk, errors);
 
         return errors;
     }

     stk->capacity = newCapacity;
#if CANARYGUARD
     stk->data = (Elem_t*) (newData + sizeof(Canary_t));
     *(getLeftDataCanary(stk))  = LeftDataCanary;
     *(getRightDataCanary(stk)) = RightDataCanary;
#else
     stk->data = (Elem_t*) newData;
#endif
     arrayPoison(stk->data + stk->size, stk->capacity - stk->size);

#if HASHGUARD
     countHashes(stk); 
#endif
 
     errors |= stackError(stk);
     stackDump(stk, errors);

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
            fprintf(DBGFILEPTR, "    Error[%d] - %s\n", index, ErrorMessages[index]);
        }

        curError <<= 1;
        index += 1;
    }

    fprintf(DBGFILEPTR, "-------------End-of-errors----------------\n");
}
#if HASHGUARD
size_t countHash(char* key, size_t len)
{
    size_t hash = noErrors;

    for (size_t i = 0; i < len; ++i) 
        hash = 33 * hash + key[i];

    return hash;
}

int checkHash(Stack_t* stk)
{
    int errors = 0;

    size_t oldStructHash = stk->debugInf.structHash;
    size_t oldDataHash   = stk->debugInf.dataHash;
    stk->debugInf.structHash = 0;
    stk->debugInf.dataHash   = 0;
    
    size_t newStructHash = countHash((char*) stk, sizeof(Stack_t));
    size_t newDataHash   = countHash((char*) stk->data, sizeof(Elem_t) * stk->capacity);
//    fprintf(DBGFILEPTR, "newDataHash %lu\n", newDataHash);
//    fprintf(DBGFILEPTR, "newStructHash %lu\n", newStructHash);
//    fprintf(DBGFILEPTR, "oldDataHash %lu\n", oldDataHash);
//    fprintf(DBGFILEPTR, "oldStructHash %lu\n", oldStructHash);

    if (newStructHash != oldStructHash) errors |= structHashError;
    if (newDataHash   != oldDataHash)   errors |= oldDataHash;

    stk->debugInf.structHash = oldStructHash;
    stk->debugInf.dataHash   = oldDataHash;

    return errors;
}

void countHashes(Stack_t* stk)
{
     stk->debugInf.dataHash   = 0;
     stk->debugInf.structHash = 0;
     size_t structHash        = countHash((char*) stk, sizeof(Stack_t));
     size_t dataHash          = countHash((char*) stk->data, sizeof(Elem_t) * stk->capacity);             
     stk->debugInf.structHash = structHash;
     stk->debugInf.dataHash   = dataHash;
}
#endif

#if CANARYGUARD
Canary_t* getLeftDataCanary(Stack_t* stk)
{
    return (Canary_t*) ((char*) stk->data - sizeof(Canary_t));
}

Canary_t* getRightDataCanary(Stack_t* stk)
{
    return (Canary_t*) ((char*) stk->data + stk->capacity * sizeof(Elem_t));
}
#endif

