#include "process_core.h"
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rstrings.h"
#include <ctype.h>

int isDigit(char a)
{
    return ('0' <= a && a <= '9');
}

int isDigitString(char a[])
{
    for(int i = 0 ; i <= strlen(a) - 1 ; i++)
    {
        if(!isDigit(a[i]))                      //checking if the whole string is made from digits
            return 0;
    }

    return 1;
}


void stringExtracter(char a[])
{
    int spaceCounter = 0;
    for(int i = 0 ; i <= strlen(a) - 1 ; i++)
    { 
        if(isspace(a[i]))
        {
            spaceCounter++;
        }
        else break;
    }
    strcpy(a, a + spaceCounter );
    a[strlen(a) - 1] = '\0';
}