#ifndef mem_manager
#define mem_manager

#include "process_core.h"

process_manager *initialAllocation();

void processReallocation(process_manager *p);
void processDeletion(process_manager *p);

#endif