#include "process_core.h"
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int isDigit(char a)
{
    return ('0' <= a && a <= '9');
}

void scanProc()
{
    DIR *dir = opendir("/proc");
    int flag;
    char procPath[50];  
    struct dirent * scann = 0;

    if(dir == NULL)
    {   
        printf("Error opening procceses");
        return;  
    }
    while((scann = readdir(dir)) != NULL)   //searching for PIDs
        {
            flag = 1;
            for(int i = 0 ; i <= strlen(scann->d_name)-1 ; i++)
            {
                if(!isDigit(scann->d_name[i]))
                    {   
                        flag = 0;
                        break;
                    }
            }
            if(flag)
            {
                makeProcess(scann->d_name, procPath);
                parser(procPath);
            }
        }
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
    char procName[50];
    char buff[256];

    err = fopen(path,"r");
 
    if(err == NULL)
    {
        printf("Error opening file.");
        return 1;
    }

    while(fgets(buff,sizeof(buff),err))
    {
        if(strncmp(buff,"Name:",5) == 0)
            {
                strcpy(procName,buff+5);
                printf("Process name: %s ",procName);
            }

    }

    if(fclose(err) != 0)
    {
        perror("Error closing file.");
        return 1;
    }

    printf("File have been closed succesfully. \n");
    return 0;
}
