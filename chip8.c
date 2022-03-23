#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

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
	int draw;
};

void initialize(struct Chip8 *chip8);

void load(char *argv, struct Chip8 *chip8);

void emulate(struct Chip8 *chip8);

void display(struct Chip8 *chip8);

void input(struct Chip8 *chip8);


/*
int main(int arg, char **argv){
	//FILE *f;
	const int width = 640;
	const int height = 320;
	
	InitWindow(width, height, "chip8");
	
	struct Chip8 chip8;
	initialize(&chip8);

	load(argv[1], &chip8);
	
	SetTargetFPS(60);
	
	while(!WindowShouldClose()){
	    if(chip8.pc<4096){
            emulate(&chip8);
	    }
	    display(&chip8);
	    input(&chip8);
	}
	
	CloseWindow();

	return 0;
}
*/


int main(int arg, char **argv){
	//FILE *f;
	
	
	struct Chip8 chip8;
	initialize(&chip8);
	
	emulate(&chip8);

	return 0;
}



void initialize(struct Chip8 *chip8){
	chip8->pc = 0x200;
	chip8->opcode = 0;
	chip8->sp = 0;
	chip8->draw = 1;
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
	    chip8->memory[i+0x50] = fonts[i];
	}
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


void emulate(struct Chip8 *chip8){
	//chip8->opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc+1];
	
	chip8->opcode = 0x6022;

	chip8->pc +=2;

	if(chip8->opcode == 0x0000){
	    return;
	}

	switch(chip8->opcode & 0xF000) {
		case 0x0000: switch(chip8->opcode & 0x0FFF){
		    
		    //clear display
			case 0x00E0: {
			    for(int i=0; i<4096; i++){
			        chip8->memory[i] = 0;
			    }
			    chip8->draw = 1;
			    break;
			}
			
			case 0x00EE: {
			    --chip8->sp;
			    chip8->pc = chip8->stack[chip8->sp];
			    break;
			}
			
			//default: printf("opcode:%04x, ins:%s\n", chip8->opcode, "0nnn"); break;	
		}
		break;
		
		//jump addr
		//checked
		case 0x1000: chip8->pc = chip8->opcode & 0x0FFF; break;
		
		//call addr
		//checked
		case 0x2000: {
			chip8->stack[chip8->sp] = chip8->pc;
			++chip8->sp;
			chip8->pc = chip8->opcode & 0x0FFF;
			break;
		}
		
		//3xkk check Vx == kk and increment pc if true
		//checked
		case 0x3000: {
			if((chip8->V[(chip8->opcode & 0x0F00) >> 8]) == (chip8->opcode & 0x00FF)){
				chip8->pc +=2;
			}
			break;
		}
		
		//4xkk check Vx != kk and increment pc
		//checked
		case 0x4000:{
			if((chip8->V[(chip8->opcode & 0x0F00) >> 8]) != (chip8->opcode & 0x00FF)){
				chip8->pc +=2;
			}
			break;
		}
		
		//5xy0 skip next instruction if Vx == Vy
		//checked
		case 0x5000:{
			if((chip8->V[(chip8->opcode & 0x0F00) >> 8]) == (chip8->V[(chip8->opcode & 0x00F0) >> 4])){
				chip8->pc +=2;
			}
			break;
		}
		
		//6xkk Vx = 6xkk
		//working
		case 0x6000:{
			chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->opcode & 0x00FF;
			break;
		}
		
		//
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
			break;
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
		
		//checked
		case 0xD000: {
		    unsigned char x = chip8->V[(chip8->opcode & 0x0F00) >> 8] % 64;
		    unsigned char y = chip8->V[(chip8->opcode & 0x00F0) >> 4] % 32;
		    unsigned char height = chip8->opcode & 0x000F;
		    
		    //printf("%d, %d\n", x, y);
		    
		    chip8->V[15] = 0;
		    
		    for(int i=0; i<height; i++){
		        unsigned char spriteByte = chip8->memory[chip8->I + i];
		        
		        for(int j=0; j<8; j++){
		            unsigned char spritePixel = spriteByte & (0x80 >> j);
		            unsigned char displayPixel = chip8->gfx[x+j][y+i];
		            
		            if(spritePixel){
		                
		                if(displayPixel == 0xFF){
		                    chip8->V[15] = 1;
		                }
		                
		                chip8->gfx[x+j][y+i] ^= 0xFF;
		                //printf("%d %d: %x\n", x+j, y+i, chip8->gfx[x+j][y+i]);
		            }
		        }
		    }
		}
		chip8->draw = 1;
		break;
		
		case 0xE000: 
    		switch(chip8->opcode & 0x00FF){
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
			
		case 0xF000: 
    		switch(chip8->opcode & 0x00FF){
    			
    			case 0x0007: {
    				chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->timer_delay;
    				break;
    			}
    
    			case 0x000A: {
    			    unsigned char Vx = (chip8->opcode & 0x0F00) >> 8;
    			    
    			    if(chip8->keys[0]){
    			        chip8->V[Vx] = 0;
    			    }
    			    else if(chip8->keys[1]){
    			        chip8->V[Vx] = 1;
    			    }
    			    else if(chip8->keys[2]){
    			        chip8->V[Vx] = 2;
    			    }
    			    else if(chip8->keys[3]){
    			        chip8->V[Vx] = 3;
    			    }
    			    else if(chip8->keys[4]){
    			        chip8->V[Vx] = 4;
    			    }
    			    else if(chip8->keys[5]){
    			        chip8->V[Vx] = 5;
    			    }
    			    else if(chip8->keys[6]){
    			        chip8->V[Vx] = 6;
    			    }
    			    else if(chip8->keys[7]){
    			        chip8->V[Vx] = 7;
    			    }
    			    else if(chip8->keys[8]){
    			        chip8->V[Vx] = 8;
    			    }
    			    else if(chip8->keys[9]){
    			        chip8->V[Vx] = 9;
    			    }
    			    else if(chip8->keys[10]){
    			        chip8->V[Vx] = 10;
    			    }
    			    else if(chip8->keys[11]){
    			        chip8->V[Vx] = 11;
    			    }
    			    else if(chip8->keys[12]){
    			        chip8->V[Vx] = 12;
    			    }
    			    else if(chip8->keys[13]){
    			        chip8->V[Vx] = 13;
    			    }
    			    else if(chip8->keys[14]){
    			        chip8->V[Vx] = 14;
    			    }
    			    else if(chip8->keys[15]){
    			        chip8->V[Vx] = 15;
    			    }
    			    else {
    			        chip8->pc -= 2;
    			    }
    			    break;
    			} 
    			
    			case 0x0015: {
    				chip8->timer_delay = chip8->V[(chip8->opcode & 0x0F00) >> 8];
    				break;
    			}
    
    			case 0x0018: {
    				chip8->sound_delay = chip8->V[(chip8->opcode & 0x0F00) >> 8];
    				break;
    			}
    			
    			//working but uncheked
    			case 0x001E: {
    				chip8->I = chip8->I + chip8->V[(chip8->opcode & 0x0F00) >> 8];
    				break;
    			}
    			
    			case 0x0029: {
    			    unsigned char digit = chip8->V[(chip8->opcode & 0x0F00) >> 8];
    			    
    			    chip8->I = 0x50 + (5 * digit);
    			} 
    			break;
    			
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
    		break;
    		
		default: printf("wrong opcode! opcode:%04x\n", chip8->opcode); break;
	}
}

/*
void display(struct Chip8 *chip8){
    
    //if(chip8->draw == 1){
        BeginDrawing();
    
        ClearBackground(BLACK);
        
        for(int rows=0; rows<32; rows++){
            for(int cols=0; cols<64; cols++){
                if(chip8->gfx[cols][rows] == 0xFF){
                    DrawRectangle(cols*10, rows*10, 10, 10, RAYWHITE);
                }
            }
        }
        
        EndDrawing();
    
}

void input(struct Chip8 *chip8){
    if(IsKeyDown(KEY_ONE)){
        chip8->V[0] = 1;
    }
    else if(IsKeyDown(KEY_TWO)){
        chip8->V[1] = 1;
    }
    else if(IsKeyDown(KEY_THREE)){
        chip8->V[2] = 1;
    }
    else if(IsKeyDown(KEY_FOUR)){
        chip8->V[3] = 1;
    }
    else if(IsKeyDown(KEY_Q)){
        chip8->V[4] = 1;
    }
    else if(IsKeyDown(KEY_W)){
        chip8->V[5] = 1;
    }
    else if(IsKeyDown(KEY_E)){
        chip8->V[6] = 1;
    }
    else if(IsKeyDown(KEY_R)){
        chip8->V[7] = 1;
    }
    else if(IsKeyDown(KEY_A)){
        chip8->V[8] = 1;
    }
    else if(IsKeyDown(KEY_S)){
        chip8->V[9] = 1;
    }
    else if(IsKeyDown(KEY_D)){
        chip8->V[10] = 1;
    }
    else if(IsKeyDown(KEY_F)){
        chip8->V[11] = 1;
    }
    else if(IsKeyDown(KEY_Z)){
        chip8->V[12] = 1;
    }
    else if(IsKeyDown(KEY_X)){
        chip8->V[13] = 1;
    }
    else if(IsKeyDown(KEY_C)){
        chip8->V[14] = 1;
    }
    else if(IsKeyDown(KEY_V)){
        chip8->V[15] = 1;
    }
    
    
    if(IsKeyUp(KEY_ONE)){
        chip8->V[0] = 1;
    }
    else if(IsKeyUp(KEY_TWO)){
        chip8->V[1] = 1;
    }
    else if(IsKeyUp(KEY_THREE)){
        chip8->V[2] = 1;
    }
    else if(IsKeyUp(KEY_FOUR)){
        chip8->V[3] = 1;
    }
    else if(IsKeyUp(KEY_Q)){
        chip8->V[4] = 1;
    }
    else if(IsKeyUp(KEY_W)){
        chip8->V[5] = 1;
    }
    else if(IsKeyUp(KEY_E)){
        chip8->V[6] = 1;
    }
    else if(IsKeyUp(KEY_R)){
        chip8->V[7] = 1;
    }
    else if(IsKeyUp(KEY_A)){
        chip8->V[8] = 1;
    }
    else if(IsKeyUp(KEY_S)){
        chip8->V[9] = 1;
    }
    else if(IsKeyUp(KEY_D)){
        chip8->V[10] = 1;
    }
    else if(IsKeyUp(KEY_F)){
        chip8->V[11] = 1;
    }
    else if(IsKeyUp(KEY_Z)){
        chip8->V[12] = 1;
    }
    else if(IsKeyUp(KEY_X)){
        chip8->V[13] = 1;
    }
    else if(IsKeyUp(KEY_C)){
        chip8->V[14] = 1;
    }
    else if(IsKeyUp(KEY_V)){
        chip8->V[15] = 1;
    }
}*/