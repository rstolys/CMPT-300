#include "mystring.h"
#include <assert.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * The following are simple tests to help you.
 * You should develop more extensive tests to try and find bugs.
 */
int main()
    {
	const int SIZE = 100;
	char buffer[SIZE];

    //Test my string length
    assert(mystrlen("") == 0);
    assert(mystrlen("cmpt 300") == 8);
    assert(mystrlen("This is a longer string to test the total length") == 48);

	mystrcpy(buffer, "");
	assert(strcmp(buffer, "") == 0);

    mystrcpy(buffer, "I am testing my string functions!");
	assert(strcmp(buffer, "I am testing my string functions!") == 0);

    assert(mystrcmp ("I love coding", "I love coding") == 0);
	assert(mystrcmp ("I love coding", "I love codingtest") == -1);
    assert(mystrcmp ("I love coding", "I love codin") == 1);
    assert(mystrcmp ("I love coding", "") == 1);
    assert(mystrcmp ("", "I love coding") == -1);
    assert(mystrcmp ("", "") == 0);


    char *dupStr = mystrdup(buffer);
    assert (!mystrcmp(buffer, dupStr));
	assert (!strcmp(buffer, dupStr));
	assert (buffer != dupStr);

	if (dupStr != NULL)
		free (dupStr);


    buffer[0] = '\0';
    char *dupStr3 = mystrdup(buffer);
    assert (!mystrcmp(buffer, dupStr3));
	assert (!strcmp(buffer, dupStr3));


	char *dupStr2 = mystrdup(NULL);
    assert (!mystrcmp(NULL, dupStr2));

	printf ("\nPassed all tests\n");

	return 0;
    }

