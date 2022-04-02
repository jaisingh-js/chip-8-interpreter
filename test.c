#include <stdio.h>


void printBinary(int num){
	int result[8];
	int n = num;
	for(int i=0; i<8; i++){
		result[i] = 0;
	}

	for(int i=0; n>0; i++){
		result[i] = n % 2;
		n = n/2;
	}

	for(int i=1; i<=8; i++){
		printf("%d", result[8-i]);
	}
    printf("\n");
}

int main(){
    
    int num = 0;

    while(num != 256){
        printf("enter number to convert: ");
        scanf("%d", &num);
        printf("\n");
        printBinary(num);    
    }

    return 0;
}