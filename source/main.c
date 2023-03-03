// Default stuff
#include <nds.h>
#include <stdio.h>

// Constants
#define MAXBULLETCOUNT 100

// My stuff
#include "GameLib.h"

// Pictures and sprites
#include "SpriteSheet.h"
#include "BasicBackground.h"

// A series of 2d arrays of pointers to the graphics memory of the sprites
u16* PlayerGFXMem[6];
u16* PlayerBulletGFXMem[4];

// Character Entity
Entity player;
int current_movement[2] = {0, 0};
int direction = 0;
int player_animation_frame_number = 0;
_Bool moving = 0;
int player_center[2];
int player_hitbox[4];

// Take a guess
int frame_number = 0;

// Bullet array and other necessary information
Bullet bullet_array[100];
int temp_bullet_hitbox[4];  // Used to hold a bullet's hitbox when handling said bullet

int alive_bullets = 0;

int bullet_delay = 10;
int current_bullet_delay = 0;

// Main boarder hitboxes
int screen_rectangle[4] = {0, 0, 256, 192};
int screen_boarder[4][4] = {
	{0, 0, 16, 192},
	{0, 0, 256, 192},
	{0, 176, 256, 16},
	{240, 0, 16, 192}
};

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	// Enable the main screen
	videoSetMode(MODE_5_2D);
	// Setting and Initalising VRAM Bank A to sprites
	vramSetBankA(VRAM_A_MAIN_SPRITE);
	oamInit(&oamMain, SpriteMapping_1D_128, false);
	// Setting and Initalising VRAM bank B to background slot 0
	vramSetBankB(VRAM_B_MAIN_BG_0x06000000);
	
	// Initalise the bottom screen for text
	consoleDemoInit();
	
	// Loading the background
	int bg3 = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
	dmaCopy(BasicBackgroundBitmap, bgGetGfxPtr(bg3), 256*256);
	dmaCopy(BasicBackgroundPal, BG_PALETTE, sizeof(BasicBackgroundPal));
	bgShow(bg3);

	// Setting the sprite palette
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
	
	// Setting up the player - void EntitySetup(Entity* self, int x, int y, int w, int h, int health, int type);
	EntitySetup(&player, 100, 100, 12, 12, 10, 0);
	EntityGetRectArray(&player, player_hitbox);
	
	// Setting up the bullet array
	for (int i = 0; i < MAXBULLETCOUNT; i++) {
		BulletInit(&bullet_array[i]);
	}
	
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
			direction = 2;
			moving = 1;
		}
		if (keys & KEY_UP){
			current_movement[1] = -1;
			direction = 1;
			moving = 1;
		}
		if (keys & KEY_DOWN) {
			current_movement[1] = 1;
			direction = 3;
			moving = 1;
		}
		
		EntityMove(&player, current_movement);
		EntityGetRectArray(&player, player_hitbox);

		for (int i = 0; i < 4; i++) {
			if (RectangleCollision(player_hitbox, screen_boarder[i])) {
				
			}
		}

		EntityGetCenterArray(&player, player_center);
		
		// Adding in bullets
		if (current_bullet_delay > 0) {
			current_bullet_delay--;
		}
		else {
			if (keys & KEY_A) {
				current_bullet_delay = bullet_delay;
				for (int i = 0; i < MAXBULLETCOUNT; i++) {
					if (!bullet_array[i].alive) {
						BulletSetup(
							&bullet_array[i],
							player_center[0] - 4, player_center[1] - 4,  // x, y
							8, 8,  // w, h
							1.5708 * direction,  // angle
							2,  // velocity
							120,  // lifespan
							1  // damage
						);
						break;
					}
				}
			}
		}
	
		// Kind of deleting old bullets and then updating them
		for (int i = 0; i < MAXBULLETCOUNT; i++) {
			if (bullet_array[i].to_die) {
				bullet_array[i].to_die = 0;
				bullet_array[i].alive = 0;
			}
			else {
				BulletUpdate(&bullet_array[i]);
				BulletGetRectArray(&bullet_array[i], temp_bullet_hitbox);  // Deleting if the bullets go outside the screen
				if (!RectangleCollision(screen_rectangle, temp_bullet_hitbox)) {
					bullet_array[i].to_die = 0;
					bullet_array[i].alive = 0;
				}
			}
		}
		
		// Counting Bullets
		alive_bullets = 0;
		for (int i = 0; i < MAXBULLETCOUNT; i++) {
			if (bullet_array[i].alive) {
				alive_bullets++;
			}
		}
		
		// Drawing in the player
		if (frame_number % 6 == 0) {
			player_animation_frame_number++;
			player_animation_frame_number = player_animation_frame_number % 4;
		}
		if (moving) {
			oamSet(
				&oamMain,
				0,
				player.x, player.y,
				0,
				0,
				SpriteSize_16x16,
				SpriteColorFormat_256Color,
				PlayerGFXMem[player_animation_frame_number],
				-1,
				false,
				false,
				false,
				false,
				false
			);
		}
		else {
			oamSet(
				&oamMain,
				0,
				player.x,
				player.y,
				0,
				0,
				SpriteSize_16x16,
				SpriteColorFormat_256Color,
				PlayerGFXMem[4 + player_animation_frame_number % 2],
				-1,
				false,
				false,
				false,
				false,
				false
			);
		}
		
		// Drawing the bullets
		for (int i = 0; i < MAXBULLETCOUNT; i++) {
			oamSet(
				&oamMain,
				28 + i,
				bullet_array[i].x,
				bullet_array[i].y,
				0,
				0,
				SpriteSize_16x16,
				SpriteColorFormat_256Color,
				PlayerBulletGFXMem[bullet_array[i].lifespan / 6 % 4],
				-1,
				false,
				!bullet_array[i].alive,
				false,
				false,
				false
			);
		}
		
		// Displaying the player position and other stuff
		iprintf("\nX = %d\nY = %d\n", (int)player.x, (int)player.y);
		iprintf("Player Center [%d, %d]\n", player_center[0], player_center[1]);
		iprintf("Direction = %d\n", direction);
		iprintf("Alive Bullets = %d\n", alive_bullets);
		iprintf("Current Bullet Delay = %d\n", current_bullet_delay);
		iprintf("%d", sizeof(BasicBackgroundBitmap));
		
		// Waiting 
		swiWaitForVBlank();
		// Update the screen
		oamUpdate(&oamMain);
		// To exit I think
		if(pressed & KEY_START) break;
	}
	
	return 0;
}

// Notes
// Screen size is 256 × 192 pixels (4:3 aspect ratio)
