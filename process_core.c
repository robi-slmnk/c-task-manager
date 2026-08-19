#include "process_core.h"
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rstrings.h"
#include "mem_manager.h"
#include <unistd.h>

void scanProc()
{
    int flag;
    char procPath[50];  
    struct dirent * scann = 0;
    process_manager *process_list;

    process_list = initialAllocation();

    while(1)
    {  
        DIR *dir = opendir("/proc");

        if(dir == NULL)
        {   
            return;  
        }
    
            printf("\e[1;1H\e[2J");
            while((scann = readdir(dir)) != NULL)   //searching for PIDs
            {
                flag = isDigitString(scann->d_name);            //checking if the PID is made only from digits

                if(flag)
                {
                    makeProcess(scann->d_name, procPath);       //assembling the path of the process if the PID is made only from digits
                    parser(procPath,process_list,scann->d_name);                           //extracting the name of the process ,
                }

            }
        process_display(process_list);
        process_list->process_count = 0;
        closedir(dir);
        sleep(1);
        }
    return ;

}

void makeProcess(char process_id[], char path[])
{

    strcpy(path,"/proc/");
    strcat(path,process_id);    //assembling the path of the process
    strcat(path,"/status");

}


int parser(char path[], process_manager *process_list, char process_id[])
{
    FILE *err;
    Process data;
    char buff[256];

    err = fopen(path,"r");
 
    if(err == NULL)
    {
        printf("Error opening file.");
        return 1;
    }

    strcpy(data.PID,process_id);

    while(fgets(buff,sizeof(buff),err))
    {
        if(strncmp(buff,"Name:",5) == 0)
            {
                strcpy(data.pName,buff+5);
                stringExtracter(data.pName);
            }
        else if(strncmp(buff,"VmRSS:",6) == 0)
            {
                strcpy(data.memory_kbb,buff+6);
                stringExtracter(data.memory_kbb);
            }


    }
    if(process_list->mem_cap == process_list->process_count)
    {
        processReallocation(process_list);
            
    }
    process_add(process_list, data);
    data.cpu_usage = 0;
    data.memory_kbb[0] = data.PID[0] = data.pName[0] = '\0';

    if(fclose(err) != 0)
    {
        perror("Error closing file.");
        return 1;
    }

    return 0;
}

void process_add(process_manager *p, Process data)
{
    p->list[p->process_count] = data;
    p->process_count ++;   
}


void process_display(process_manager *process_list)
{
    if(process_list->list)
    {
        for(int i = 0 ; i < process_list->process_count ; i++)
            printf("Process ID   %s   Name   %s  Memory   %s \n", process_list->list[i].PID, process_list->list[i].pName, process_list->list[i].memory_kbb);
    }
    else printf("There are no processes running!\n");
}