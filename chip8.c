#include <stdio.h>

struct Chip8 {
	unsigned char memory[4096];
	unsigned char V[16];
	unsigned short I;
	unsigned short pc;
	unsigned char timer_delay, sound_delay;
	unsigned short gfx[64][32];
	unsigned short opcode;
};

void initialize(struct Chip8 *chip8){
	chip8->pc = 0;
	chip8->opcode = 0;
	for(int i=0; i<4096; i++){
		chip8->memory[i] = 0;
	}
	for(int i=0; i<16; i++){
		chip8->V[i] = 0;
	}
	
	for(int i=0; i<64; i++){
		for(int j=0; j<32; j++){
			chip8->gfx[i][j] = 0;
		}
	}
	chip8->I = 0;
	chip8->timer_delay = 0;
	chip8->sound_delay = 0;
}

int main(int arg, char **argv){
	FILE *f;
	struct Chip8 chip8;
	char *pMem;

	f = fopen(argv[1], "rb");
	
	if(f==NULL){
		printf("Error! file does not exist!");
		return 1;
	}

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	initialize(&chip8);
	pMem = chip8.memory;
	fread(pMem + 0x200, 1, fsize, f);
	
	fclose(f);
	
	for(int i=0; i<4096; i++){
		printf("%x ", chip8.memory[i]);
	}

	return 0;
}
