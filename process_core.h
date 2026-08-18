#ifndef process_core
#define process_core

#define MAX_NAME_LENGTH 256

#include <stdlib.h>
#include <dirent.h>

typedef struct{
    int PID; 
    char pName[30];
    long memory_kbb;
    float cpu_usage;
    
}Process;

Process* scanProc();
void printProc(Process* Proc);

#endif