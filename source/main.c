// Default stuff
#include <nds.h>
#include <stdio.h>
#include <math.h>

// My stuff
#include "GameLib.h"

// Pictures and sprites
#include "SpriteSheet.h"
#include "BasicBackground.h"

// Constants
#define MAXBULLETCOUNT 100
#define PI 3.14159265359

// A series of 2d arrays of pointers to the graphics memory of the sprites
u16* PlayerGFXMem[6];
u16* PlayerBulletGFXMem[4];

// Character Entity
Entity player;
int player_movement[2];
int direction = 0;  // This is technically a global variable as it is used and edited within functions, i dont really like this but i cant be bothered to rectify it
int player_animation_frame_number = 0;
_Bool moving = 0;
int player_center[2];
int player_hitbox[4];

void PlayerMovement(Entity* self, int keys, int HitboxArray[][4], int HitboxLen, int movement_array[2]) {
	int player_hitbox[4];
	// X Movement
	int x = 0;
	if (keys & KEY_RIGHT) {
		x = 1;
		direction = 0;
	}
	if (keys & KEY_LEFT) {
		x = -1;
		direction = 2;
		}
	// Moving the player
	self->x += x;
	// Collision Detection if the player actually moved
	if (x != 0) {
		EntityGetRectArray(self, player_hitbox);
		for (int i = 0; i < HitboxLen; i++) {
			if (RectangleCollision(player_hitbox, HitboxArray[i])) {  // If a collision
				// Adjust the player position accordingly
				if (x > 0) {
					EntitySetRight(self, HitboxArray[i][0]);
				}
				else {
					self->x = RectangleGetRight(HitboxArray[i]);
				}
				// Update the hitbox
				EntityGetRectArray(self, player_hitbox);
			}
		}
	}
	
	// Y Movement
	int y = 0;
	if (keys & KEY_DOWN) {
		y = 1;
		direction = 3;
	}
	if (keys & KEY_UP) {
		y = -1;
		direction = 1;
	}
	// Moving the player
	self->y +=y ;
	// Collision Detection if the player actually moved
	if (y != 0) {
		EntityGetRectArray(self, player_hitbox);
		for (int i = 0; i < HitboxLen; i++) {
			if (RectangleCollision(player_hitbox, HitboxArray[i])) {  // If a collision
				// Adjust the player position accordingly
				if (y > 0) {
					EntitySetBottom(self, HitboxArray[i][1]);
				}
				else {
					self->y = RectangleGetBottom(HitboxArray[i]);
				}
				// Update the hitbox
				EntityGetRectArray(self, player_hitbox);
			}
		}
	}

	// Passing the movement to the movement array
	movement_array[0] = x;
	movement_array[1] = y;
}

void PlayerFireBullet(Entity* self, int keys, int bullet_delay, int* current_bullet_delay, Bullet bullet_array[], int bullet_array_len) {
	if (*current_bullet_delay > 0) {
			*current_bullet_delay = *current_bullet_delay - 1;
		}
	else {
		if (keys & KEY_A) {
			*current_bullet_delay = bullet_delay;  // Reset the bullet delay
			BulletSetupInBulletArray(
				bullet_array, MAXBULLETCOUNT,  // Bullet array information
				player_center[0] - 4, player_center[1] - 4,  // x, y
				8, 8,  // w, h
				PI / 2 * direction,  // angle
				2,  // velocity
				120,  // lifespan
				1  // damage
			);
		}
	}
}

void PlayerAnimate(Entity* self, int frame_number, int* player_animation_frame_number, u16* player_gfx_mem[], _Bool moving) {
	if (frame_number % 6 == 0) {
			*player_animation_frame_number = *player_animation_frame_number + 1;
			*player_animation_frame_number = *player_animation_frame_number % 4;
	}
	if (moving) {
		oamSet(
			&oamMain,
			0,
			self->x, self->y,
			0,
			0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			player_gfx_mem[*player_animation_frame_number],
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
			self->x, self->y,
			0,
			0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			player_gfx_mem[4 + *player_animation_frame_number % 2],
			-1,
			false,
			false,
			false,
			false,
			false
		);
	}
}

// Take a guess
int frame_number = 0;

// Bullet array and other necessary information
Bullet bullet_array[100];
int temp_bullet_hitbox[4];  // Used to hold a bullet's hitbox when handling said bullet

int alive_bullets = 0;

int bullet_delay = 10;
int current_bullet_delay = 0;

// Main boarder hitboxes
int playable_area[4] = {0, 0, 256, 192};
int screen_boarder[4][4] = {
	{0, 0, 8, 192},
	{0, 0, 256, 8},
	{0, 184, 256, 8},
	{248, 0, 8, 192}
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
	
	// Setting up the player
	EntitySetup(&player, 100, 100, 10, 10, 10, 0);
	EntityGetRectArray(&player, player_hitbox);
	
	// Setting up the bullet array
	BulletInitBulletArray(bullet_array, MAXBULLETCOUNT);
	
	while(1) {
		// Clear the text
		consoleClear();
		// Get key presses
		scanKeys();
		int keys = keysHeld();
		int pressed = keysDown();
		// Frame number
		frame_number++;
		frame_number %= 60;
		
		// Player movement
		PlayerMovement(&player, keys, screen_boarder, 4, player_movement);
		if (player_movement[0] || player_movement[1]) {
			moving = 1;
		}
		else {
			moving = 0;
		}

		// Collecting player infomation
		EntityGetRectArray(&player, player_hitbox);
		EntityGetCenterArray(&player, player_center);
		
		// Adding in bullets
		PlayerFireBullet(&player, keys, bullet_delay, &current_bullet_delay, bullet_array, MAXBULLETCOUNT);
	
		// Kind of 'deleting' old bullets and then updating them
		BulletHandleBulletArray(bullet_array, MAXBULLETCOUNT, playable_area);
		
		// Counting Bullets
		alive_bullets = 0;
		for (int i = 0; i < MAXBULLETCOUNT; i++) {
			if (bullet_array[i].alive) {
				alive_bullets++;
			}
		}
		
		// Drawing and animating the player
		PlayerAnimate(&player, frame_number, &player_animation_frame_number, PlayerGFXMem, moving);
		
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
		// iprintf("%d\n", keys);
		// iprintf("%d, %d, %d, %d\n", KEY_RIGHT, KEY_LEFT, KEY_UP, KEY_DOWN);
		// iprintf("%d, %d, %d, %d", keys & KEY_RIGHT, keys & KEY_LEFT, keys & KEY_UP, keys & KEY_DOWN);
		
		// Waiting 
		swiWaitForVBlank();
		// Update the screen
		oamUpdate(&oamMain);
		// To exit
		if(pressed & KEY_START) break;
	}
	
	return 0;
}

// Notes
// Screen size is 256 × 192 pixels (4:3 aspect ratio)
