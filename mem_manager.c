#include <stdio.h>
#include <stdlib.h>
#include "process_core.h"
#include "mem_manager.h"

process_manager *initialAllocation()
{
    process_manager *p;
    p = malloc(sizeof(process_manager));
    p->list = (Process*) malloc(10 * sizeof(Process));
    p->mem_cap = 10;
    p->process_count = 0;
    if(p->list == NULL)
    {
        printf("Error! Failed to allocate memory.");
        exit(0);
    }
    return p;

}


void processReallocation(process_manager *p)
{
    Process *temp;
    temp = realloc(p->list,2 * p->mem_cap * sizeof(Process));

    if(temp == NULL)
    {
        printf("Erorr! Failed to reallocate memory.");
        exit(0);
    }
    p->list = temp;
    p->mem_cap = 2 * p->mem_cap;
}
void processDeletion(process_manager *p)
{
    if(p != NULL)
    {
        if(p->list != NULL)
        {
            free(p->list);
            p->list = NULL;
        }
        free(p);       
    }
}

int sort_string(const void *a, const void *b)
{
    Process *A = (Process*)a;
    Process *B = (Process*)b;

    int len_a, len_b;
    
    len_a = strlen(A->memory_kbb);
    len_b = strlen(B->memory_kbb);
    
    if(len_a == len_b)
    {
        return strcmp(B->memory_kbb, A->memory_kbb);
    }
    else
    {
        if(len_a > len_b)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }
}