#pragma once

#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <cstring>

#ifndef LOGNAME
#define LOGNAME "logfile.txt"
#endif

void logprint(char param);
void logprint(int param);
void logprint(const char* param);
void logprint(double param);

FILE* const logOpen(const char*);
extern FILE* const LOGFILEPTR;

#define $ do { fprintf(LOGFILEPTR, "FILE: %s, Function: %s, line: %d\n", __FILE__, __func__, __LINE__); } while(0)

#define $$(param) do { logprint(#param); logprint(" Return value = "); logprint(param); logprint('\n'); } while(0)

