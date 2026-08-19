#ifndef process_core
#define process_core

#define MAX_NAME_LENGTH 256

#include <stdlib.h>
#include <dirent.h>

typedef struct{
    char PID[64]; 
    char pName[64];
    char memory_kbb[64];
    float cpu_usage;
    
}Process;

typedef struct{
    Process *list;
    int process_count;
    int mem_cap;
}process_manager;

void scanProc();
void makeProcess(char process_id[], char path[]);
void process_add(process_manager *p, Process data);
void process_display(process_manager *process_list, int max_screen_height, int offset);

int display_settings();
int parser(char path[],process_manager *process_list, char process_id[]);

#endif