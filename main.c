#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "beavalloc.h" 

int main(int argc, char *argv[])
{
    char *array0, *array1, *array2, *array3, *array4;
    int i;

    beavalloc_set_verbose(1);

    array0 = beavalloc(1024*sizeof(char));
    for(i = 0; i<1024; ++i){
        array0[i] = '!';
    }

    array1 = beavalloc(1024 * sizeof(char));
    for (i = 0; i < 1024; ++i){
        array1[i] = '!';
    }

    array2 = beavalloc(1024 * sizeof(char));
    for (i = 0; i < 1024; ++i)
    {
        array2[i] = '!';
    }

    array3 = beavalloc(1024 * sizeof(char));
    for (i = 0; i < 1024; ++i)
    {
        array3[i] = '!';
    }

    array4 = beavalloc(1024 * sizeof(char));
    for (i = 0; i < 1024; ++i)
    {
        array4[i] = '!';
    }

    beavfree(array2);
    beavfree(array3);
    // beavalloc_reset();

    // array = beavcalloc(400, sizeof(int));
    
    // msg1 = beavalloc(600 * sizeof(char));
    // strcpy(msg1, "Hello World Again");
    // msg2 = beavalloc(25 * sizeof(char));
    // strcpy(msg2, "Hello World A third time");
    // printf("%s\n", msg2);

    beavalloc_reset();

    return 0;
}