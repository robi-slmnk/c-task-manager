#include "process_core.h"
#include <stdio.h>
#include <ncurses.h>

int main()
{
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(100);  
    scanProc();
    return 0;
}