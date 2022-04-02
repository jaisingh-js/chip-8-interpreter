#include <stdio.h>
#include <raylib.h>


struct Chip8{
    unsigned char memory[4096];
	unsigned char V[16];
	unsigned short I;
	unsigned short pc;
	unsigned char delay_timer, sound_timer;
	unsigned short stack[16];
	unsigned char sp;
	unsigned char gfx[32][64];
	unsigned char key[16];
	unsigned short opcode;
};

void load(char *argv, struct Chip8 *chip8);

void initialize(struct Chip8 *chip8);

void emulate(struct Chip8 *chip8);

void display(struct Chip8 *chip8);

void printBinary(int num);


int main(int arg, char **argv){

	const int width = 640;
	const int height = 320;
	int releaseKey = 0;

	InitWindow(width, height, "chip8 emulator");

	struct Chip8 chip8;
	initialize(&chip8);
	load(argv[1], &chip8);

	/*
	chip8.opcode = (chip8.memory[chip8.pc] << 8) | chip8.memory[chip8.pc+1];
	printf("opcode : %04x\n", chip8.opcode);
	printf("pc before : %04x\n", chip8.pc);
	emulate(&chip8);
	printf("pc after : %04x\n", chip8.pc);
	*/


	SetTargetFPS(60);
	
	while(!WindowShouldClose()){
		chip8.opcode = (chip8.memory[chip8.pc] << 8) | chip8.memory[chip8.pc+1];
		
		/*
		if(IsKeyDown(KEY_RIGHT) && (releaseKey == 0)){
			emulate(&chip8);
			releaseKey = 1;
		}
		else if(IsKeyUp(KEY_RIGHT)){
			releaseKey = 0;
		}
		*/
		emulate(&chip8);
		
		display(&chip8);
	}
	
	CloseWindow();

	return 0;
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


void initialize(struct Chip8 *chip8){
    
    for(int i=0; i<4096; i++){
        chip8->memory[i] = 0;
    }
    
    for(int i=0; i<16; i++){
       chip8->V[i] = 0;   
    }
    
    chip8->I = 0;
    chip8->pc = 0x200;
    chip8->delay_timer = 0;
    chip8->sound_timer = 0;
    
    for(int i=0; i<16; i++){
        chip8->stack[i] = 0;
    }
    
    chip8->sp = 0;
    
    for(int row=0; row <32; row++){
		for(int col=0; col<64; col++){
			chip8->gfx[row][col] = 0;
		}
	}

	for(int i=0; i<16; i++){
		chip8->key[i] = 0;
	}

	chip8->opcode = 0;
}


void emulate(struct Chip8 *chip8){
	unsigned short op = chip8->opcode;
	//printf("opcode: %04x\n", op);
	unsigned short opc = op & 0xF000;
	
	if(opc == 0x0000){
		if((op & 0x00FF) == 0x00E0){
			//00E0 clear display
			//checked
			for(int row=0; row <32; row++){
				for(int col=0; col<64; col++){
					chip8->gfx[row][col] = 0;
				}
			}
			//printf("pc before: %04x\n", chip8->pc);
			chip8->pc += 2;
			//printf("pc after: %04x\n", chip8->pc);
		}
	}

	else if(opc == 0x1000){
		//1NNN set pc to NNN
		//checked
		unsigned short address = op & 0x0FFF;
		//printf("pc before : %04x\n", chip8->pc);
		chip8->pc = address;
		//printf("pc after: %04x\n", chip8->pc);
	}

	else if(opc == 0x6000){
		//6XNN set Vx to NN
		unsigned char x = (op & 0x0F00) >> 8;
		unsigned char address = op & 0x00FF;
		
		chip8->V[x] = address;
		//printf("pc before : %04x\n", chip8->pc);
		chip8->pc += 2;
		//printf("pc after: %04x\n", chip8->pc);
	}

	else if(opc == 0x7000){
		//7XNN add NN to Vx
		unsigned char x = (op & 0x0F00) >> 8;
		unsigned char address = op & 0x00FF;

		chip8->V[x] += address;
		//printf("pc before : %04x\n", chip8->pc);
		chip8->pc += 2;
		//printf("pc after: %04x\n", chip8->pc);
	}

	else if(opc == 0xA000){
		//ANNN set I to NNN
		unsigned short address = op & 0x0FFF;

		chip8->I = address;
		//printf("pc before : %04x\n", chip8->pc);
		chip8->pc += 2;
		//printf("pc after: %04x\n", chip8->pc);
	}

	else if(opc == 0xD000){
		//DXYN draw sprite at x,y of n height
		unsigned char x = (op & 0x0F00) >> 8;
		unsigned char y = (op & 0x00F0) >> 4;
		unsigned char height = op & 0x000F;

		unsigned char Vx = chip8->V[x];
		unsigned char Vy = chip8->V[y];
		unsigned char pixel = 0;

		chip8->V[0xF] = 0;

		for(int row=0; row<height; row++){
			pixel = chip8->memory[chip8->I + row];
			//printBinary(pixel);

			for(int col=0; col<8; col++){
				unsigned char pixel_bit = pixel & (0x80 >> col);
				//printBinary(pixel_bit);
				unsigned char screen_pixel = chip8->gfx[Vy+row][Vx+col];

				if(pixel_bit != 0){
					if(screen_pixel == 1){
						chip8->V[0xF] = 1;
					}

					chip8->gfx[Vy+row][Vx+col] ^= 1;
				}
			}
			//printf("\n");
		}
		/*
		for(int row=0; row <32; row++){
			for(int col=0; col<64; col++){
				printf("%d",chip8->gfx[row][col]);
			}
			printf("\n");
		}
		*/
		//printf("pc before : %04x\n", chip8->pc);
		chip8->pc += 2;
		//printf("pc after: %04x\n", chip8->pc);
	}

	else {
		printf("unknown opcode: %04x\n", op);
	}


}


void display(struct Chip8 *chip8){

	BeginDrawing();

	ClearBackground(BLACK);
	
	for(int row=0; row<32; row++){
		for(int col=0; col<64; col++){
			if(chip8->gfx[row][col] == 1){
				DrawRectangle(col*10, row*10, 10, 10, RAYWHITE);
			}
		}
	}

	EndDrawing();
}


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