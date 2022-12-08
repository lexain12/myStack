#include <math.h>
#include "logs/LogLib.h"

#define CANARYGUARD 0
#define HASHGUARD 0

#ifndef DBGFILENAME
#define DBGFILENAME "debugFile.txt"
#endif

#define stackDump(stkptr, errors) stackDumpFunc(stkptr, errors, __LINE__, __func__, __FILE__, DBGFILEPTR);
#define stackCtor(stkptr, size)   stackCtorFunc(stkptr, size, #stkptr, __func__, __FILE__, __LINE__);

void print(char param);
void print(int param);
void print(const char* param);
void print(double param);

//const Elem_t POISON = {12, 228};

// const Elem_t POISON = (char *) (0x1000 - 0x7);

int getPoison(int elem);
float getPoison(float elem);
double getPoison(double elem);
char getPoison(char elem);
char* getPoison(char* elem);

const size_t DefaultCapacity    = 10;

const size_t DestructionValue   = 0xDED32DED;
#if CANARYGUARD
const Canary_t LeftCanary       = 0xDEADBEEFDEADBEEF;
const Canary_t RightCanary      = 0xDEDAFADEDEDAFADE;
const Canary_t LeftDataCanary   = 0xCAFEBABECAFEBABE;
const Canary_t RightDataCanary  = 0xCAFED00DCAFED00D;
#endif

enum Mode 
{
    DOWN = 1,
    UP   = 2,
};

FILE* dbgOpen(const char* filename); // trash

extern FILE* DBGFILEPTR;

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
    size_t       bornLine;
    StackStatus  stackStatus;
#if HASHGUARD
    size_t       dataHash;
    size_t       structHash;
#endif
};

struct Stack_t
{
#if CANARYGUARD
    Canary_t leftCanary = LeftCanary;
#endif
    size_t   size;
    size_t   capacity; 
    Elem_t*  data;
    DebugInf debugInf;
#if CANARYGUARD
    Canary_t rightCanary = RightCanary;
#endif
};

enum Errors 
{
    noErrors             = 0 << 0,
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

void print (FILE* file, Elem_t element);
//-------------------------------------------------------
