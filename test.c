#include <stdio.h>
#include <raylib.h>

int main(){

	const int width = 500;
	const int height = 500;

	int key = 0;

	InitWindow(width, height, "test");

	SetTargetFPS(60);

	while(!WindowShouldClose()){
		
		//key = GetKeyPressed();
		if(IsKeyDown(49)){
			printf("1 down");
		}

		BeginDrawing();

		ClearBackground(PURPLE);

		EndDrawing();

	}

	CloseWindow();
    return 0;
}