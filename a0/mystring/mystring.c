#include "mystring.h"
#include <stdio.h>
#include <stdlib.h>     //For malloc

/*
 *   Implement the following functions: 
 * 
 *   You are NOT allowed to use any standard string functions such as 
 *   strlen, strcpy or strcmp or any other string function.
 */

/*
 *  mystrlen() calculates the length of the string s, 
 *  not including the terminating character '\0'.
 *  Returns: length of s.
 */
int mystrlen (const char *s) 
    {
	/*Get the length of the array*/
    int numChars = 0; 

    //Until we reach the terminating character
    while(s[0] != '\0')
        {
        numChars++;
        s = &s[1];        //Move pointer to next position
        }

	return numChars;
    }

/*
 *  mystrcpy()  copies the string pointed to by src (including the terminating 
 *  character '\0') to the array pointed to by dst.
 *  Returns: a  pointer to the destination string dst.
 */
char  *mystrcpy (char *dst, const char *src)
    {
    int c = 0;

    //Assumes there is enough space in dst for the desired copy operation

    //While we are not at the terminating character
    while(src[c] != '\0')
        {
        dst[c] = src[c];        //Set the destination string character to the correct character
        c++;                    //Increment to the next character
        }

    //Append the terminating character 
    dst[c] = '\0';

	return dst;
    }

/*
 * mystrcmp() compares two strings alphabetically
 * Returns: 
 * 	-1 if s1  < s2
 *  	0 if s1 == s2
 *  	1 if s1 > s2
 */
int mystrcmp(const char *s1, const char *s2)
    {
    int rv = 0;             //Return Value
	int sameString = 1;     //using as boolean
    int s1Val = 0; 
    int s2Val = 0; 

    int c = 0;              

    //While both strings still have characters remaining 
    while(s1[c] != '\0' && s2[c] != '\0')
        {
        if(s1[c] != s2[c])
            sameString = 0;         //Show they are not the same string 

        s1Val += (int) s1[c];       //Increment counter for string 1
        s2Val += (int) s2[c];       //Increment counter for string 2

        c++;                        //Move to next character
        }
    //
    // One of the strings has not reached its end
    //

    //If s1 has not been completed
    if(s1[c] != '\0')
        {
        sameString = 0;         //s1 and s2 are not the same string

        //Count up remaining total for string 1
        while(s1[c] != '\0')
            {
            s1Val += (int) s1[c];       //Increment counter for string 2
            c++;
            }
        }
    //If s2 has not been completed 
    else if(s2[c] != '\0')     
        {
        sameString = 0;         //s1 and s2 are not the same string

        //Count up remaining total for string 2
        while(s2[c] != '\0')
            {
            s2Val += (int) s2[c];       //Increment counter for string 2
            c++;
            }
        }
    //else  both strings have terminated 


    //Determine return value 
    if(sameString)
        {rv = 0;}
    else if(s1Val > s2Val)
       	{rv = 1;}
    else            //NOTE: this means that if we have different strings with the same UNIX sum then we return -1
       	{rv = -1;}

    return rv;
    }

/*
 * mystrdup() creates a duplicate of the string pointed to by s. 
 * The space for the new string is obtained using malloc.  
 * If the new string can not be created, a null pointer is returned.
 * Returns:  a pointer to a new string (the duplicate) 
 	     or null If the new string could not be created for 
	     any reason such as insufficient memory.
 */
char *mystrdup(const char *s1)
    {
    char *dst = NULL;       //Pointer to hold memory for next string
    int len = 0;            //Will determine the length of the string
    int c = 0;              //counter for character in string

    if(s1 != NULL)
        {
        //get length of string
        len = mystrlen(s1);

        //If string is more than a terminating character
        if(len > 0)
            {
            //Allocate memory for 
            dst = (char *) malloc(mystrlen(s1) + 1);     //making character pointer so length is multiplied by sizeof(char) == 1

            //Assign values of s1 to dst 
            while(s1[c] != '\0')
                {
                dst[c] = s1[c];
                c++;
                }

            //Add terminating character 
            dst[c] = '\0';
            }
        }
    

	return dst;
    }

