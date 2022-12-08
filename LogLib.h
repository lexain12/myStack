#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <cstring>

#ifndef LOGNAME
#define LOGNAME "logfile.txt"
#endif

void logprint(char param);
void logprint(int param);
void logprint(double param);
void logprint(const char* param);

FILE* logOpen(const char*);

extern FILE* LOGFILEPTR;

#define $ fprintf(LOGFILEPTR, "FILE: %s, Function: %s, line: %d\n", __FILE__, __func__, __LINE__);

#define $$(param) param; logprint(#param); logprint(param); logprint('\n'); \

