#include "process_core.h"
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
void scanProc()
{
    DIR *dir = opendir("/proc");  
    if(dir == NULL)
        return;  
    struct dirent * scann = 0;
    while((scann = readdir(dir)) != NULL)   //searching for PIDs
        printf("File name: %s \n",scann->d_name);
    closedir(dir);
    return ;

}
void makeProcess(char process_id[], char path[])
{

    strcpy(path,"/proc/");
    strcat(path,process_id);    //assembling the path of the process
    strcat(path,"/status");

}
int parser(char path[])
{
    FILE *err;
    err = fopen(path,"r");
    if(err == NULL)
    {
        printf("Error opening file.");
        return 1;
    }
    char procName[50];
    char procMem[50];
    char buff[256];
    while(fgets(buff,sizeof(buff),err))
    {
        if(strncmp(buff,"Name:",5) == 0)
            strcpy(procName,buff+5);

    }

    if(fclose(err) != 0)
    {
        perror("Error closing file.");
        return 1;
    }

    printf("File have been closed succesfully. \n");
    return 0;
}
void printProc(Process* Proc);