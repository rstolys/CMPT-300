#include <stdio.h>
#include <stdarg.h>

//Might be too complicated -- revisit later
/*******************************************************************
** myWrite -- will print to screen using write command -- only supports int values to add
**
** @param[in]  fd       file descriptor to write to
** @param[in]  num      specifies the number of input arguments passed
**
** Code Referenced from: https://codeforwin.org/2017/09/variable-length-arguments-c.html 
********************************************************************/
/*
void myWrite(int fd, int num, ...)
    {
    va_list list; 
    va_start(list, num);

    int numInts = 0;
    char *buf;
    int values[10];     //Only allow 10 values maximum

    //Need at least 1 varaible argument to use
    if(num > 0)
        {
        //Loop over all the arguements 
        for(int a = 0; a < num; a++)
            {
            if(numInts)
                {
                int values[a - 1] = va_arg(list, int);

                numInts--;
                }
            else
                {
                char *buf = va_arg(list, char*);

                //Loop through string to determine if more characters are expected
                for(int c = 0; c < strlen(buf); c++)
                    {
                    if(buf[c] == "%")
                        {
                        if(buf[c + 1] == "d")
                            {
                            numInts++;
                            }
                        }
                    }
                }
            


            }
        }

    //Clean the list
    va_end(list);

    return;
    }
*/