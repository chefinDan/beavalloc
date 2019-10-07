#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "beavalloc.h" 

int main(int argc, char *argv[])
{
    char *msg, *msg1;

    beavalloc_set_verbose(1);

    msg = beavalloc(500 * sizeof(char));
    strcpy(msg, "Hello World");
    // printf("%s\n", msg);

    msg1 = beavalloc(500 * sizeof(char));
    strcpy(msg1, "Hello World Again");
    // printf("%s\n", msg1);

    beavalloc_reset();

    msg = beavalloc(400 * sizeof(char));
    strcpy(msg, "Hello World");
    // printf("%s\n", msg);

    msg1 = beavalloc(600 * sizeof(char));
    strcpy(msg1, "Hello World Again");
    // msg2 = beavalloc(25 * sizeof(char));
    // strcpy(msg2, "Hello World A third time");
    // printf("%s\n", msg2);

    beavalloc_reset();

    return 0;
}