#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "beavalloc.h"

int main(int argc, char *argv[])
{
    char *msg;
    char *msg1;
    // if(argc < 2) { fprintf(stderr, "Usage: beavalloc <integer>\n"); exit(1); }
    msg = beavalloc(12 * sizeof(char));
    strcpy(msg, "Hello World");
    printf("%s\n", msg);

    msg1 = beavalloc(18 * sizeof(char));
    strcpy(msg1, "Hello World Again");
    printf("%s\n", msg1);

    return 0;
}