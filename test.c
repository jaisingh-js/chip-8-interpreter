#include <stdio.h>

int main(){
    
    unsigned char opcode = 0xf21e & 0x001e;
    
    switch(opcode){
        case 0x001e:
            printf("working");
            break;
    }

    return 0;
}