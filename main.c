#include "beavalloc.h"

int main(int arg, char *argv[])
{
    char *input;

    input = beavalloc(strlen(argv[1]) * sizeof(char));
    strcpy(input, argv[1]);

    printf("%s", input);

    return 0;
}