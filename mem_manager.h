#ifndef mem_manager
#define mem_manager

#include "process_core.h"
#include <string.h>

process_manager *initialAllocation();

void processReallocation(process_manager *p);
void processDeletion(process_manager *p);

int sort_string(const void *a, const void *b);

#endif