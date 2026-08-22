#include "process_core.h"
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rstrings.h"
#include "mem_manager.h"
#include <unistd.h>
#include <ncurses.h>
#include <signal.h>

void scanProc()
{
    int flag, max_screen_height, offset, pressed_key;
    char procPath[50];  
    struct dirent * scann = 0;
    process_manager *process_list;
    int marker = 0;
    int refresh_speed = 1000;
    process_stat new, old, delta;
    float procent = 0;

    offset = 0;
    process_list = initialAllocation();
    new = stats_parser();

    while(1)
    {  
        old.active = new.active;
        old.idle = new.idle;
        process_list->process_count = 0;
        DIR *dir = opendir("/proc");

        if(dir == NULL)
        {   
            return;  
        } 
        clear();
        max_screen_height = display_settings();
        mvprintw(3, 1, "Process ID %9s Name %-35s Memory %-20s"," ", " ", " ");
        mvprintw(1, 0,"Press P or p for process pause     Press K or k for process termination         Press Q or q to close the task manager");

        while((scann = readdir(dir)) != NULL)   //searching for PIDs
        {
            flag = isDigitString(scann->d_name);            //checking if the PID is made only from digits

            if(flag)
            {
                makeProcess(scann->d_name, procPath);       //assembling the path of the process if the PID is made only from digits
                process_parser(procPath,process_list,scann->d_name);                           //extracting the name of the process ,
            }

        }
        qsort(process_list->list, process_list->process_count, sizeof(Process), sort_string);
        process_display(process_list, max_screen_height, offset, marker, procent);
        closedir(dir);
        if(marker >= process_list->process_count)
                marker = process_list->process_count-1;
        pressed_key = getch();
        new = stats_parser(); 
        switch(pressed_key)
        {
            case KEY_UP:
                if(marker > 0)
                {
                    marker--;
                }
                if(offset > 0 && (marker < offset))
                {
                    offset --;
                }
                break;
            case KEY_DOWN:
                if(marker < process_list->process_count - 1)
                {
                    marker++;
                }
                if(marker >= offset + max_screen_height - 4)
                {
                    offset++;
                }
                break;
            case 'p':
            case 'P':
                refresh_speed = -refresh_speed;
                timeout(refresh_speed);
                break;
            case 'k':
            case 'K':
                end_process(process_list, marker);
                break;
            case 'q':
            case 'Q':
                processDeletion(process_list);
                endwin();
                return;
                break;   
        }
        delta.active = new.active - old.active;
        delta.idle = new.idle - old.idle;
        procent = ((delta.active - delta.idle)/(float)(delta.active)) * 100;
        
        refresh();
    }
    return ;

}

void makeProcess(char process_id[], char path[])
{

    strcpy(path,"/proc/");
    strcat(path,process_id);    //assembling the path of the process
    strcat(path,"/status");

}


int process_parser(char path[], process_manager *process_list, char process_id[])
{
    FILE *err;
    Process data;
    char buff[256];

    err = fopen(path,"r");
 
    if(err == NULL)
    {
        mvprintw(0, 2, "Error opening file.");
        return 1;
    }

    data.cpu_usage = 0;
    strcpy(data.memory_kbb, "0 kB"); 
    data.PID[0] = data.pName[0] = '\0';
    strcpy(data.PID,process_id);

    while(fgets(buff,sizeof(buff),err))
    {
        if(strncmp(buff,"Name:",5) == 0)
            {
                strcpy(data.pName,buff+5);
                stringExtracter(data.pName);
            }
        if(strncmp(buff,"VmRSS:",6) == 0)
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

    if(fclose(err) != 0)
    {
        mvprintw(0, 2, "Error closing file.");
        return 1;
    }

    return 0;
}

void process_add(process_manager *p, Process data)
{
    p->list[p->process_count] = data;
    p->process_count ++;   
}

int display_settings()
{
    int maxy, maxx;
    
    getmaxyx(stdscr,maxy,maxx);
    return maxy;
}

void process_display(process_manager *process_list, int max_screen_height, int offset, int marker, float percentage)
{
    mvprintw(0,0,"Processor usage : %f ", percentage);
    if(process_list->list)
    {
        int current = 0, flag = 0;;
        for(int i = 0 ; i < max_screen_height - 1 && (i + offset) < process_list->process_count ; i++)
        {
            flag = 0;
            current = i + offset;
            if(current == marker  )
            {
                flag = 1;
                attron(A_REVERSE);
            }
            mvprintw(i+4, 1,"%-20s %-40s %-20s", process_list->list[current].PID, process_list->list[current].pName, process_list->list[current].memory_kbb);
            if(flag)
            {
                attroff(A_REVERSE);
            }
        }
    }
    else mvprintw(0, 2, "There are no processes running!");
} 

void end_process(process_manager *process_list, int marker)
{
    if(process_list->list)
    {
        long long t_id;

        t_id = atoi(process_list->list[marker].PID);
        kill(t_id, 9);   
    }
    else mvprintw(0, 2, "There are no processes running!");
} 


process_stat stats_parser()
{
    
    FILE *proc;
    char all_time[256];
    char *errHandler = 0;
    process_stat data; 

    proc = fopen("/proc/stat", "r");

    if(proc == NULL)
    {
        exit(0);
    }

    errHandler = fgets(all_time, sizeof(all_time), proc);
    fclose(proc);
    if(errHandler == NULL)
    {
        exit(0);
    }
    data = process_math(all_time);
    return data;
}

process_stat process_math(char buff[])
{
    char *process_time;
    int num_count = 0;
    unsigned long long sti;
    process_stat data;

    data.active = data.idle = 0;
    process_time = strtok(buff, " ");
    process_time = strtok(NULL, " ");

    while(process_time != NULL)
    {
        num_count ++;
        sti = atoll(process_time);
        data.active += sti;
        if(num_count == 4 || num_count == 5 )
        {
            data.idle += sti;
        }
        process_time = strtok(NULL, " ");
    }

    return data;
}