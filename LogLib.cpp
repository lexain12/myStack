#include <time.h>
#include <stdlib.h>
#include "LogLib.h"

tm Tm = {};

FILE* LOGFILEPTR = logOpen(LOGNAME);

void logprint(char param)
{
    fprintf(LOGFILEPTR, "%c", param);
}

void logprint(int param)
{
    fprintf(LOGFILEPTR, "%d", param);
}

void logprint(const char* param)
{
    fprintf(LOGFILEPTR, "%s", param);
}

void logprint(double param)
{
    fprintf(LOGFILEPTR, "%lg", param);
}

FILE* logOpen(const char* logFileName)
{
    char buf[0] = {};
    FILE* filePtr = fopen(logFileName, "a");
    setvbuf(filePtr, buf, _IONBF, 0);

    const time_t timer = time(NULL);
    fprintf(filePtr, "---------------%s", strtok(ctime(&timer), "\n"));
    fseek(filePtr, -1, SEEK_CUR);
    fprintf(filePtr, "---------------\n");

    return filePtr;
}

