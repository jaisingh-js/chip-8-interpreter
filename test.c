#include <stdio.h>

int main(){

    int num = 255;
    int a, b, c;

    a = num/100;
    printf("%d\n", a);

    b = (num%100)/10;
    printf("%d\n", b);

    c = num % 10;
    printf("%d\n", c);

    return 0;
}