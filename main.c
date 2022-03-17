#include <stdio.h>

int main(){
	unsigned char memory[4096];
	unsigned char V[16];
	unsigned short I;
	unsigned short pc = 200;
	unsigned char delay_timer, sound_timer;
	unsigned short stack[16];
	unsigned short sp;
	unsigned short opcode;
	unsigned char gfx[64 * 32];
	unsigned char key[16];

	opcode = 0xA010;

	switch(opcode & 0xF000){
		case 0xA000:
			I = opcode & 0x0FFF;
			pc = pc+2;
			//printf("%016x\n", 0x0A10);
			printf("%04x\n", I);
			printf("%d\n", pc);
			break;
		default:
			printf("wrong opcode");
			break;
	}


	return 0;
		
}
