#include<stdio.h>

int main(void) {
    int a = 0x80000000 >> 31;
    printf("%d", a);

    return 0;
}
