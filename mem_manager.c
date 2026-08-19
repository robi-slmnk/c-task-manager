#include <stdio.h>
#include <stdlib.h>
#include "process_core.h"

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
    if(p->list)
    {
        for(int i = 0 ; i <= p->process_count ; i++)
        {
            free(p->list);
            p->list = NULL;
        }
        free(p->list);
    }

}