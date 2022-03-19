#include <stdio.h>
#include <stdlib.h>

struct Chip8 {
	unsigned char memory[4096];
	unsigned char V[16];
	unsigned short I;
	unsigned short pc;
	unsigned char timer_delay, sound_delay;
	unsigned short gfx[64][32];
	unsigned short opcode;
	unsigned short stack[16];
	unsigned short sp;
	unsigned char keys[16];
};

void initialize(struct Chip8 *chip8);

void load(char *argv, struct Chip8 *chip8);

void emulate(struct Chip8 *chip8, unsigned short pc);


int main(int arg, char **argv){
	//FILE *f;
	struct Chip8 chip8;
	initialize(&chip8);

	load(argv[1], &chip8);
	
	/*
	for(int i=0; i<4096; i++){
		printf("%x ", chip8.memory[i]);
	}
	*/
	printf("\n");

	while(chip8.pc<4096){
		emulate(&chip8, chip8.pc);
		printf("\n%d\n", chip8.sp);
		chip8.pc += 2;
	}

	return 0;
}


void initialize(struct Chip8 *chip8){
	chip8->pc = 0x200;
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


void load(char *argv, struct Chip8 *chip8) {
	FILE *f;
	unsigned char *pMem;

	f = fopen(argv, "rb");
	
	if(f==NULL){
		printf("Error! file does not exist!");
	}

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	pMem = chip8->memory;
	fread(pMem + 0x200, 1, fsize, f);
	
	fclose(f);
}

void dissemble(struct Chip8 *chip8, unsigned short pc){
	chip8->opcode = (chip8->memory[pc] << 8) | chip8->memory[pc+1];

	if(chip8->opcode == 0x0000){
		return;
	}
	//printf("%04x\n", chip8->opcode & 0xF000);

	switch(chip8->opcode & 0xF000) {
		case 0x0000: switch(chip8->opcode & 0x0FFF){
			case 0x00E0: printf("opcode:%04x, ins:%s\n", chip8->opcode, "00E0"); break;
			case 0x00EE: printf("opcode:%04x, ins:%s\n", chip8->opcode, "00EE"); break;
			default: printf("opcode:%04x, ins:%s\n", chip8->opcode, "0nnn"); break;	
		}
			break;
		case 0x1000: printf("opcode:%04x, ins:%s\n", chip8->opcode, "1nnn"); break;
		case 0x2000: printf("opcode:%04x, ins:%s\n", chip8->opcode, "2nnn"); break;
		case 0x3000: printf("opcode:%04x, ins:%s\n", chip8->opcode, "3xkk"); break;
		case 0x4000: printf("opcode:%04x, ins:%s\n", chip8->opcode, "4xkk"); break;
		case 0x5000: printf("opcode:%04x, ins:%s\n", chip8->opcode, "5xy0"); break;
		case 0x6000: printf("opcode:%04x, ins:%s\n", chip8->opcode, "6xkk"); break;
		case 0x7000: printf("opcode:%04x, ins:%s\n", chip8->opcode, "7xkk"); break;
		case 0x8000: switch(chip8->opcode & 0x000F){
			case 0x0000: printf("opcode:%04x, ins:%s\n", chip8->opcode, "8xy0"); break;
			case 0x0001: printf("opcode:%04x, ins:%s\n", chip8->opcode, "8xy1"); break;
			case 0x0002: printf("opcode:%04x, ins:%s\n", chip8->opcode, "8xy2"); break;
			case 0x0003: printf("opcode:%04x, ins:%s\n", chip8->opcode, "8xy3"); break;
			case 0x0004: printf("opcode:%04x, ins:%s\n", chip8->opcode, "8xy4"); break;
			case 0x0005: printf("opcode:%04x, ins:%s\n", chip8->opcode, "8xy5"); break;
			case 0x0006: printf("opcode:%04x, ins:%s\n", chip8->opcode, "8xy6"); break;
			case 0x0007: printf("opcode:%04x, ins:%s\n", chip8->opcode, "8xy7"); break;
			case 0x000E: printf("opcode:%04x, ins:%s\n", chip8->opcode, "8xyE"); break;
		}
			break;
		case 0x9000: printf("opcode:%04x, ins:%s\n", chip8->opcode, "9xy0"); break;
		case 0xA000: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Annn"); break;
		case 0xB000: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Bnnn"); break;
		case 0xC000: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Cxkk"); break;
		case 0xD000: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Dxyn"); break;
		case 0xE000: switch(chip8->opcode & 0x00FF){
			case 0x009E: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Ex9E"); break;
			case 0x00A1: printf("opcode:%04x, ins:%s\n", chip8->opcode, "ExA1"); break;
		}
			break;
		case 0xF000: switch(chip8->opcode &0x00FF){
			case 0x0007: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Fx07"); break;
			case 0x000A: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Fx0A"); break;
			case 0x0015: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Fx15"); break;
			case 0x0018: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Fx18"); break;
			case 0x001E: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Fx1E"); break;
			case 0x0029: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Fx29"); break;
			case 0x0033: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Fx33"); break;
			case 0x0055: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Fx55"); break;
			case 0x0065: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Fx65"); break;
		}
		default: printf("wrong opcode! opcode:%04x\n", chip8->opcode); break;
	}
}


void emulate(struct Chip8 *chip8, unsigned short pc){
	chip8->opcode = (chip8->memory[pc] << 8) | chip8->memory[pc+1];

	if(chip8->opcode == 0x0000){
		return;
	}
	//printf("%04x\n", chip8->opcode & 0xF000);

	switch(chip8->opcode & 0xF000) {
		case 0x0000: switch(chip8->opcode & 0x0FFF){
			case 0x00E0: printf("clear display"); break;
			case 0x00EE: printf("opcode:%04x, ins:%s\n", chip8->opcode, "00EE"); break;
			default: printf("opcode:%04x, ins:%s\n", chip8->opcode, "0nnn"); break;	
		}
			break;
		case 0x1000: pc = chip8->opcode & 0x0FFF; break;
		case 0x2000: {
			chip8->stack[chip8->sp] = pc;
			++chip8->sp;
			chip8->pc = chip8->opcode & 0x0FFF;
			break;
		}
		case 0x3000: {
			if((chip8->V[(chip8->opcode & 0x0F00) >> 8]) == (chip8->opcode & 0x00FF)){
				chip8->pc +=2;
			}
			break;
		}
		case 0x4000:{
			if((chip8->V[(chip8->opcode & 0x0F00) >> 8]) != (chip8->opcode & 0x00FF)){
				chip8->pc +=2;
			}
			break;
		}
		case 0x5000:{
			if((chip8->V[(chip8->opcode & 0x0F00) >> 8]) == (chip8->V[(chip8->opcode & 0x00F0) >> 4])){
				chip8->pc +=2;
			}
			break;
		}
		case 0x6000:{
			chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->opcode & 0x00FF;
			break;
		}
		case 0x7000:{
			chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] + chip8->opcode & 0x00FF;
			break;
		}
		case 0x8000: switch(chip8->opcode & 0x000F){
			case 0x0000: {
				chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x00F0) >> 4];
				break;
			}
			case 0x0001:{
				chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] | chip8->V[(chip8->opcode & 0x00F0) >> 4];
				break;
			}
			case 0x0002:{
				chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] & chip8->V[(chip8->opcode & 0x00F0) >> 4];
				break;
			}
			case 0x0003:{
				chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] ^ chip8->V[(chip8->opcode & 0x00F0) >> 4];
				break;
			}
			case 0x0004: {
				if((chip8->V[(chip8->opcode & 0x0F00) >> 8] + chip8->V[(chip8->opcode & 0x00F0) >> 4]) > 0xFF){
					chip8->V[15] = 1;
				}
				else{
					chip8->V[15] = 0;
				}

				chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] + chip8->V[(chip8->opcode & 0x00F0) >> 4];
				break;
			}

			case 0x0005: {
				if(chip8->V[(chip8->opcode & 0x0F00) >> 8] > chip8->V[(chip8->opcode & 0x00F0) >> 4]){
					chip8->V[15] = 1;
				}
				else{
					chip8->V[15] = 0;
				}

				chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] - chip8->V[(chip8->opcode & 0x00F0) >> 4];
				break;
			}

			case 0x0006: {
				if((chip8->V[(chip8->opcode & 0x0F00) >>8] << 7 & 0b10000000) == 0b10000000){
					chip8->V[15] = 1;
				}
				else {
					chip8->V[15] = 0;
				}
				chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] / 2;
				break;
			}

			case 0x0007: {
				if(chip8->V[(chip8->opcode & 0x00F0) >> 4] > chip8->V[(chip8->opcode & 0x0F00) >> 8]){
					chip8->V[15] = 1;
				}
				else{
					chip8->V[15] = 0;
				}

				chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x00F0) >> 4] - chip8->V[(chip8->opcode & 0x0F00) >> 8];
				break;
			}

			case 0x000E: {
				if((chip8->V[(chip8->opcode & 0x0F00) >> 8] >> 7 & 0b00000001) == 0b00000001){
					chip8->V[15] = 1;
				}
				else {
					chip8->V[15] = 0;
				}
				chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x0F00) >> 8] * 2;
				break;
			}
			break;
		}

		case 0x9000: {
			if(chip8->V[(chip8->opcode & 0x0F00) >> 8] != chip8->V[(chip8->opcode & 0x00F0) >> 4]){
				chip8->pc += 2;
			}
		}
		case 0xA000: {
			chip8->I = chip8->opcode & 0x0FFF;
			break;
		}
		case 0xB000: {
			chip8->pc = (chip8->opcode & 0x0FFF) + chip8->V[0];
			break;
		}
		case 0xC000: {
			chip8->V[(chip8->opcode & 0x0F00) >> 8] = (rand()%255) & (chip8->opcode & 0x00FF);
			break;
		}
		case 0xD000: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Dxyn"); break;
		case 0xE000: switch(chip8->opcode & 0x00FF){
			case 0x009E: {
				if(chip8->keys[chip8->V[(chip8->opcode & 0x0F00) >> 8]] == 1){
					chip8->pc += 2;
				}
			}
			case 0x00A1: {
				if(chip8->keys[chip8->V[(chip8->opcode & 0x0F00) >> 8]] == 0){
					chip8->pc += 2;
				}
			}
		}
			break;
		case 0xF000: switch(chip8->opcode &0x00FF){
			
			case 0x0007: {
				chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->timer_delay;
				break;
			}

			case 0x000A: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Fx0A"); break;
			
			case 0x0015: {
				chip8->timer_delay = chip8->V[(chip8->opcode & 0x0F00) >> 8];
				break;
			}

			case 0x0018: {
				chip8->sound_delay = chip8->V[(chip8->opcode & 0x0F00) >> 8];
				break;
			}
			case 0x001E: {
				chip8->I = chip8->I + chip8->V[(chip8->opcode & 0x0F00) >> 8];
				break;
			}
			
			case 0x0029: printf("opcode:%04x, ins:%s\n", chip8->opcode, "Fx29"); break;
			
			case 0x0033: {
				chip8->memory[chip8->I] = chip8->V[(chip8->opcode & 0x0F00) >> 8] / 100;
				chip8->memory[chip8->I + 1] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] % 100) / 10;
				chip8->memory[chip8->I + 2] = chip8->V[(chip8->opcode & 0x0F00) >> 8] % 10;
				break;

			}

			case 0x0055: {
				for(int i=0; i<16; i++){
					chip8->memory[chip8->I + i] = chip8->V[i];
				}
				break;
			}
			
			case 0x0065: {
				for(int i=0; i<16; i++){
					chip8->V[i] = chip8->memory[chip8->I + i];
				}
				break;
			}
		}
		default: printf("wrong opcode! opcode:%04x\n", chip8->opcode); break;
	}
}