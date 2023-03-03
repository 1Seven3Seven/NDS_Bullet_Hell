// Default stuff
#include <nds.h>
#include <stdio.h>

// Constants
#define MAXBULLETCOUNT 100

// My stuff
#include "GameLib.h"

// Pictures and sprites
#include "SpriteSheet.h"

// A 2d array of pointers to the graphics memory of the Character
u16* PlayerGFXMem[6];
u16* PlayerBulletGFXMem[4];

// Character Entity
Entity Player;
int current_movement[2] = {0, 0};
int direction = 0;
int PlayerAnimationFrameNumber = 0;
_Bool moving = 0;

// Take a guess
int frame_number = 0;

// Bullet array and other necessary information
Bullet BulletArray[MAXBULLETCOUNT];
int BulletAnimationFrameNumber = 0;
enum {PlayerBullet = 0, EnemyBullet = 1};

int current_bullet_delay = 0;
int bullet_delay = 5;

int BulletsFired = 0;

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	// Enable the main screen with background 0 active
	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	// Setting and Initalising VRAM Bank A to sprites
	vramSetBankA(VRAM_A_MAIN_SPRITE);
	oamInit(&oamMain, SpriteMapping_1D_128, false);
	
	// Initalise the bottom screen for text
	consoleDemoInit();
	
	// Setting the palette
	dmaCopy(SpriteSheetPal, SPRITE_PALETTE, 512);
	
	// Allocating memory for, and loading the player
	for (int a = 0; a < 6; a++) {
		PlayerGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + 16*16 * a, PlayerGFXMem[a], 16 * 16);
	}
	// Allocating memory for, and loading the character bullets
	for (int a = 0; a < 4; a++) {
		PlayerBulletGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + 16*16*8*7 + 16*16 * a, PlayerBulletGFXMem[a], 16 * 16);
	}
	
	// Setting up the player - void SetupEntity(Entity* self, int x, int y, int w, int h, int health, int type);
	SetupEntity(&Player, 100, 100, 8, 8, 10, 0);
	
	// Setting up the bullet array - void SetupBullet(Bullet* self, float x, float y, int w, int h, float angle, int damage, int type);
	for (int i = 0; i < MAXBULLETCOUNT; i++) {
		BulletInit(&BulletArray[i]);
	}
	SetupBullet(&BulletArray[0], 0, 0, 315, 1, 1, 180, PlayerBullet);

	while(1) {
		// Clear the text
		consoleClear();
		// Get key presses
		scanKeys();
		int keys = keysHeld();
		int pressed = keysDown();
		// Frame number
		frame_number++;
		frame_number = frame_number % 60;
		
		// Player movement
		current_movement[0] = 0;  // Reset the movement
		current_movement[1] = 0;
		moving = 0;
		if (keys & KEY_RIGHT) {
			current_movement[0] = 1;
			direction = 0;
			moving = 1;
		}
		if (keys & KEY_LEFT) {
			current_movement[0] = -1;
			direction = 1;
			moving = 1;
		}
		if (keys & KEY_UP){
			current_movement[1] = -1;
			direction = 2;
			moving = 1;
		}
		if (keys & KEY_DOWN) {
			current_movement[1] = 1;
			direction = 3;
			moving = 1;
		}
		
		PlayerMove(&Player, current_movement);
		
		// Adding in bullets
		if (current_bullet_delay > 0) {
			current_bullet_delay--;
		}
		else {
			if (keys & KEY_A) {
				BulletsFired++;
				for (int i = 0; i < MAXBULLETCOUNT; i++) {
					if (BulletArray[i].alive == 0) {
						SetupBullet(&BulletArray[i], Player.x, Player.y, 0, 1, 1, 120, PlayerBullet);
						break;
					}
				}
				current_bullet_delay = bullet_delay;
			}
		}
		
		// Kind of deleting old bullets and then updating them
		for (int i = 0; i < MAXBULLETCOUNT; i++) {
			if (BulletArray[i].to_delete) {
				BulletArray[i].to_delete = 0;
				BulletArray[i].alive = 0;
			}
			else {
				if (BulletArray[i].alive) {
					BulletUpdate(&BulletArray[i]);
				}
			}
		}
		
		// Drawing in the player
		if (frame_number % 6 == 0) {
			PlayerAnimationFrameNumber++;
			PlayerAnimationFrameNumber = PlayerAnimationFrameNumber % 4;
		}
		if (moving) {
			oamSet(
				&oamMain, 0, Player.x, Player.y, 1, 0, SpriteSize_16x16, SpriteColorFormat_256Color, PlayerGFXMem[PlayerAnimationFrameNumber], -1, false, false, false, false, false
			);
		}
		else {
			oamSet(
				&oamMain, 0, Player.x, Player.y, 1, 0, SpriteSize_16x16, SpriteColorFormat_256Color, PlayerGFXMem[4 + PlayerAnimationFrameNumber % 2], -1, false, false, false, false, false
			);
		}
		
		// Drawing the bulletS
		if (frame_number % 4 == 0) {
			BulletAnimationFrameNumber++;
			BulletAnimationFrameNumber = BulletAnimationFrameNumber % 4;
		}
		for (int i = 0; i < MAXBULLETCOUNT; i++) {
			oamSet(
				&oamMain,
				28 + i,  // The sprite id to be set
				(int)BulletArray[i].x,
				(int)BulletArray[i].y,
				1,  // Priority from 0 to 3
				0,  // Palette alpha, idk
				SpriteSize_16x16,
				SpriteColorFormat_256Color,
				PlayerBulletGFXMem[BulletAnimationFrameNumber],
				-1,  // Affine index to use (if < 0 or > 31 the sprite will be unrotated)
				false,  // Size double
				BulletArray[i].alive,  //  Hide, if non zero then it will be hidden
				false,  // vertical flip
				false,  // horizontal flip
				false  // mosaic
			);
			
		}
		
		// Displaying the player position
		iprintf("\nX = %d\nY = %d\n", (int)Player.x, (int)Player.y);
		iprintf("[%d, %d]\n", current_movement[0], current_movement[1]);
		iprintf("Direction = %d\n", direction);
		iprintf("Bullet = %d\n", BulletArray[0].to_delete);
		iprintf("BulletsFired = %d\n", BulletsFired);
		
		// Waiting 
		swiWaitForVBlank();
		oamUpdate(&oamMain);
		// To exit I think
		if(pressed & KEY_START) break;
	}
	
	return 0;
}

// Notes
// Screen size is 256 × 192 pixels (4:3 aspect ratio)
