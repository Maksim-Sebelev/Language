#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{

    const char* str = "+12";
    char* end = nullptr;
    int a = strtol(str, &end, 10);

    printf("a = '%d'\n", a);
    printf("dif ptr = %ld\n", end - str);


    printf("a = %d\n", a);


    return 0;
}
