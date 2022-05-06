#include <stdio.h>
#include <stdlib.h>
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
	unsigned char drawFlag;
};

void load(char *argv, struct Chip8 *chip8);

void initialize(struct Chip8 *chip8);

void emulate(struct Chip8 *chip8);

void display(struct Chip8 *chip8);

void printBinary(int num);

void input(struct Chip8 *chip8);


int main(int arg, char **argv){

	const int width = 960;
	const int height = 320;

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
		

		/*
		if(IsKeyDown(KEY_RIGHT) && (releaseKey == 0)){
			emulate(&chip8);
			releaseKey = 1;
		}
		else if(IsKeyUp(KEY_RIGHT)){
			releaseKey = 0;
		}
		*/

		
		for(int i=0; i<10; i++){
			chip8.opcode = (chip8.memory[chip8.pc] << 8) | chip8.memory[chip8.pc+1];
			emulate(&chip8);
		}

		if(chip8.delay_timer > 0){
			chip8.delay_timer--;
		}
		if(chip8.sound_timer > 0){
			chip8.sound_timer--;
		}
		

		display(&chip8);

		input(&chip8);
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

	chip8->drawFlag = 0;

	char fonts[] = {
	    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    	0x20, 0x60, 0x20, 0x20, 0x70, // 1
    	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	for(int i=0; i<80; i++){
	    chip8->memory[i] = fonts[i];
	}
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
			chip8->drawFlag = 1;

			chip8->pc += 2;
			//printf("pc after: %04x\n", chip8->pc);
		}

		else if((op & 0x00FF) == 0x00EE){
			//00EE return from subroutine
			--chip8->sp;
			chip8->pc = chip8->stack[chip8->sp];

			chip8->pc += 2;
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

	else if(opc == 0x2000){
		//2NNN call subroutine at NNN
		unsigned short address = op & 0x0FFF;
		chip8->stack[chip8->sp] = chip8->pc;
		chip8->sp++;
		chip8->pc = address;
	}

	else if(opc == 0x3000){
		//3xNN skip ins if Vx = NN
		unsigned char x = (op & 0x0F00) >> 8;
		unsigned char address = op & 0x00FF;

		if(chip8->V[x] == address){
			chip8->pc += 2;
		}

		chip8->pc += 2;
	}

	else if(opc == 0x4000){
		//4xNN skip ins if Vx != NN
		unsigned char x = (op & 0x0F00) >> 8;
		unsigned short address = op & 0x00FF;

		if(chip8->V[x] != address){
			chip8->pc += 2;
		}

		chip8->pc += 2;
	}

	else if(opc == 0x5000){
		//5xy0 skip ins if Vx == Vy
		unsigned char x = (op & 0x0F00) >> 8;
		unsigned char y = (op & 0x00F0) >> 4;

		if(chip8->V[x] == chip8->V[y]){
			chip8->pc += 2;
		}

		chip8->pc += 2;
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


	else if(opc == 0x8000){
		
		opc = op & 0x000F;

		if(opc == 0x0000){
			//8xy0 set Vx = Vy
			unsigned char x = (op & 0x0F00) >> 8;
			unsigned char y = (op & 0x00F0) >> 4;

			chip8->V[x] = chip8->V[y];

			chip8->pc += 2;
		}

		else if(opc == 0x0001){
			//8xy1 set Vx = Vx | Vy
			unsigned char x = (op & 0x0F00) >> 8;
			unsigned char y = (op & 0x00F0) >> 4;

			chip8->V[x] = chip8->V[x] | chip8->V[y];

			chip8->pc += 2;
		}

		else if(opc == 0x0002){
			//8xy2 set Vx = Vx & Vy
			unsigned char x = (op & 0x0F00) >> 8;
			unsigned char y = (op & 0x00F0) >> 4;

			chip8->V[x] = chip8->V[x] & chip8->V[y];

			chip8->pc += 2;

		}

		else if(opc == 0x0003){
			//8xy3 set Vx = Vx ^ Vy
			unsigned char x = (op & 0x0F00) >> 8;
			unsigned char y = (op & 0x00F0) >> 4;

			chip8->V[x] = chip8->V[x] ^ chip8->V[y];

			chip8->pc += 2;
		}

		else if(opc == 0x0004){
			//8xy4 set Vx = Vx + Vy and set Vf
			unsigned char x = (op & 0x0F00) >> 8;
			unsigned char y = (op & 0x00F0) >> 4;

			unsigned short result = chip8->V[x] + chip8->V[y];

			if(result > 255){
				chip8->V[15] = 1;
			}
			else{
				chip8->V[15] = 0;
			}

			chip8->V[x] = result & 0xFF;

			chip8->pc += 2;
		}

		else if(opc == 0x0005){
			//8xy5 set Vx = Vx - Vy
			unsigned char x = (op & 0x0F00) >> 8;
			unsigned char y = (op & 0x00F0) >> 4;

			if(chip8->V[x] > chip8->V[y]){
				chip8->V[15] = 1;
			}
			else{
				chip8->V[15] = 0;
			}

			chip8->V[x] = chip8->V[x] - chip8->V[y];

			chip8->pc += 2;
		}

		else if(opc == 0x0006){
			//8xy6 set Vx = Vy then shift Vx >> 1 and set that value to Vf
			unsigned char x = (op & 0x0F00) >> 8;

			if((chip8->V[x] & 0x1) == 0x1){
				chip8->V[15] = 1;
			}
			else{
				chip8->V[15] = 0;
			}

			chip8->V[x] = chip8->V[x] >> 1;

			chip8->pc += 2;

		}

		else if(opc == 0x0007){
			//8xy5 set Vx = Vx - Vy and set Vf
			unsigned char x = (op & 0x0F00) >> 8;
			unsigned char y = (op & 0x00F0) >> 4;

			unsigned char Vx = chip8->V[x];
			unsigned char Vy = chip8->V[y];

			if(Vy > Vx){
				chip8->V[15] = 1;
			}
			else{
				chip8->V[15] = 0;
			}

			chip8->V[x] = Vy - Vx;

			chip8->pc += 2;
		}

		else if(opc == 0x000E){
			//8xyE set Vx = Vy then shift Vx << 1 and set that value to Vf
			unsigned char x = (op & 0x0F00) >> 8;

			if((chip8->V[x] & 0x80) == 0x80){
				chip8->V[15] = 1;
			}
			else{
				chip8->V[15] = 0;
			}

			chip8->V[x] = chip8->V[x] << 1;

			chip8->pc += 2;

		}
		

	}

	else if(opc == 0x9000){
		//9xy0 skip ins if Vx != Vy
		unsigned char x = (op & 0x0F00) >> 8;
		unsigned char y = (op & 0x00F0) >> 4;

		if(chip8->V[x] != chip8->V[y]){
			chip8->pc += 2;
		}

		chip8->pc += 2;
	}

	else if(opc == 0xA000){
		//ANNN set I to NNN
		unsigned short address = op & 0x0FFF;

		chip8->I = address;
		//printf("pc before : %04x\n", chip8->pc);
		chip8->pc += 2;
		//printf("pc after: %04x\n", chip8->pc);
	}

	else if(opc == 0xB000){
		//BNNN set pc = NNN + V[0]
		unsigned short address = op & 0x0FFF;
		chip8->pc = address + chip8->V[0];
	}

	else if(opc == 0xC000){
		//CxNN set Vx = rand() + NN
		unsigned char randnum = rand() % 256;
		unsigned char address = op & 0x00FF;
		unsigned char x = (op & 0x0F00) >> 8;

		chip8->V[x] = randnum & address;
		
		chip8->pc += 2;
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
		chip8->drawFlag = 1;
		chip8->pc += 2;
		//printf("pc after: %04x\n", chip8->pc);
	}

	else if (opc == 0xE000){

		opc = op & 0x00FF;

		if(opc == 0x009E){
			//Ex9E  skip ins if key[Vx] = 1
			unsigned char x = (op & 0x0F00) >> 8;
			unsigned char Vx = chip8->V[x];

			if(chip8->key[Vx] == 1){
				chip8->pc += 2;
			}

			chip8->pc += 2;
		}

		else if(opc == 0x00A1){
			//ExA1  skip ins if key[Vx] != 1
			unsigned char x = (op & 0x0F00) >> 8;
			unsigned char Vx = chip8->V[x];

			if(chip8->key[Vx] == 0){
				chip8->pc += 2;
			}

			chip8->pc += 2;
		}

	}

	else if(opc == 0xF000){

		opc = op & 0x00FF;

		if(opc == 0x0007){
			//Fx07 set Vx = delay_timer
			unsigned char x = (op & 0x0F00) >> 8;
			chip8->V[x] = chip8->delay_timer;

			chip8->pc += 2;
		}

		else if(opc == 0x0015){
			//Fx15 set delay_timer = Vx
			unsigned char x = (op & 0x0F00) >> 8;
			chip8->delay_timer = chip8->V[x];

			chip8->pc += 2;
		}

		else if(opc == 0x0018){
			//Fx18 set sound_timer = Vx
			unsigned char x = (op & 0x0F00) >> 8;
			chip8->sound_timer = chip8->V[x];

			chip8->pc += 2;
		}

		else if(opc == 0x001E){
			//Fx1E set I += Vx and set Vf = 1 if I>0x0FFF
			unsigned char x = (op & 0x0F00) >> 8;
			chip8->I += chip8->V[x];

		/*
			if(chip8->I > 0x0FFF){
				chip8->V[15] = 1;
			}
			*/

			chip8->pc += 2;
		}

		else if(opc == 0x000A){
			//Fx0A wait for key and put Vx = key number
			unsigned char x = (op & 0x0F00) >> 8;
			int keypressed = 0;
			
			for(int i=0; i<16; i++){
				if(chip8->key[i] == 1){
					chip8->V[x] = i;
					keypressed = 1;
				}
			}
			if(keypressed == 0){
				return;
			}

			chip8->pc += 2;
		}

		else if(opc == 0x0029){
			//Fx29 set I to digit in Vx
			unsigned char x = (op & 0x0F00) >> 8;
			unsigned char digit = chip8->V[x] & 0x0F;

			chip8->I =  (5 * digit);

			chip8->pc += 2;
		}

		else if(opc == 0x0033){
			//Fx33 store decimal digits of Vx in I I+1 I+2
			unsigned char x = (op & 0x0F00) >> 8;
			unsigned char digit = chip8->V[x];

			chip8->memory[chip8->I] = digit / 100;
			chip8->memory[chip8->I + 1] = (digit % 100) / 10;
			chip8->memory[chip8->I + 2] = digit % 10;

			chip8->pc += 2;

		}
		

		else if(opc == 0x0055){
			//Fx55 store registers to memory
			unsigned char x = (op & 0x0F00) >> 8;

			for(int i=0; i<=x; i++){
				chip8->memory[chip8->I + i] = chip8->V[i];
			}

			chip8->pc += 2;
		}

		else if(opc == 0x0065){
			//Fx65 load registers from memory
			unsigned char x = (op & 0x0F00) >> 8;

			for(int i=0; i<=x; i++){
				chip8->V[i] = chip8->memory[chip8->I + i];
			}

			chip8->pc += 2;
		}
	}

	else {
		printf("unknown opcode: %04x\n", op);
	}


}


void display(struct Chip8 *chip8){

	BeginDrawing();

	ClearBackground(BLACK);
	if(chip8->drawFlag == 1){
		for(int row=0; row<32; row++){
			for(int col=0; col<64; col++){
				if(chip8->gfx[row][col] == 1){
					DrawRectangle(col*10, row*10, 10, 10, RAYWHITE);
				}
			}
		}
	}

	for(int i=0; i<16; i++){
		int row = i / 4;
		int col = i % 4;
		if(chip8->key[i] == 1){
			DrawText("1", 640+col*80, row*80, 20, RAYWHITE);
		}
		else{
			DrawText("0", 640+ col*80, row*80, 20, RAYWHITE);
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


void input(struct Chip8 *chip8){
	if(IsKeyDown(KEY_ONE)){
        chip8->key[0] = 1;
    }
    if(IsKeyDown(KEY_TWO)){
        chip8->key[1] = 1;
    }
    if(IsKeyDown(KEY_THREE)){
        chip8->key[2] = 1;
    }
    if(IsKeyDown(KEY_FOUR)){
        chip8->key[3] = 1;
    }
    if(IsKeyDown(KEY_Q)){
        chip8->key[4] = 1;
    }
    if(IsKeyDown(KEY_W)){
        chip8->key[5] = 1;
    }
    if(IsKeyDown(KEY_E)){
        chip8->key[6] = 1;
    }
    if(IsKeyDown(KEY_R)){
        chip8->key[7] = 1;
    }
    if(IsKeyDown(KEY_A)){
        chip8->key[8] = 1;
    }
    if(IsKeyDown(KEY_S)){
        chip8->key[9] = 1;
    }
    if(IsKeyDown(KEY_D)){
        chip8->key[10] = 1;
    }
    if(IsKeyDown(KEY_F)){
        chip8->key[11] = 1;
    }
    if(IsKeyDown(KEY_Z)){
        chip8->key[12] = 1;
    }
    if(IsKeyDown(KEY_X)){
        chip8->key[13] = 1;
    }
    if(IsKeyDown(KEY_C)){
        chip8->key[14] = 1;
    }
    if(IsKeyDown(KEY_V)){
        chip8->key[15] = 1;
    }

	if(IsKeyUp(KEY_ONE)){
        chip8->key[0] = 0;
    }
    if(IsKeyUp(KEY_TWO)){
        chip8->key[1] = 0;
    }
    if(IsKeyUp(KEY_THREE)){
        chip8->key[2] = 0;
    }
    if(IsKeyUp(KEY_FOUR)){
        chip8->key[3] = 0;
    }
    if(IsKeyUp(KEY_Q)){
        chip8->key[4] = 0;
    }
    if(IsKeyUp(KEY_W)){
        chip8->key[5] = 0;
    }
    if(IsKeyUp(KEY_E)){
        chip8->key[6] = 0;
    }
    if(IsKeyUp(KEY_R)){
        chip8->key[7] = 0;
    }
    if(IsKeyUp(KEY_A)){
        chip8->key[8] = 0;
    }
    if(IsKeyUp(KEY_S)){
        chip8->key[9] = 0;
    }
    if(IsKeyUp(KEY_D)){
        chip8->key[10] = 0;
    }
    if(IsKeyUp(KEY_F)){
        chip8->key[11] = 0;
    }
    if(IsKeyUp(KEY_Z)){
        chip8->key[12] = 0;
    }
    if(IsKeyUp(KEY_X)){
        chip8->key[13] = 0;
    }
    if(IsKeyUp(KEY_C)){
        chip8->key[14] = 0;
    }
    if(IsKeyUp(KEY_V)){
        chip8->key[15] = 0;
    }
}