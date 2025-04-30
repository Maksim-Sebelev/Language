#include <stdio.h>
#include <stdlib.h>

int main()
{
    int a = 2;

    a = 2*a++;

    printf("a = %d\n", a);

    return a;
}
