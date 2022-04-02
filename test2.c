#include <stdio.h>

int main(){

    for(int i=0; i<64*32; i++){
        int row = i / 64;
        int col = i % 64;

        printf("(%d, %d)", row, col);
    }

    return 0;
}