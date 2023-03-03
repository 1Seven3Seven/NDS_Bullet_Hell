/*
ToDo:
	Get the basic enemy working, animations, movement and all

	Redo the player animation so it loads the sprite every time it changes, this is mainly to conserve GFX

	Improve player sprite so it isnt directional
	
	Potentially do some research into sounds
		They make everything better
*/

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
#define PLAYERBULLET 0
#define ENEMYBASICBULLET 1
#define ENEYMINEBULLET 2

// A series of 2d arrays of pointers to the graphics memory of the sprites
u16* PlayerGFXMem[8];
u16* BulletGFXMem[4][4];

// Player Entity
Entity player;
int player_movement[2];
int direction = 0;  // This is technically a global variable as it is used and maybe edited within functions, i dont really like this but i cant be bothered to rectify it
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
	self->y += y;
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

void PlayerFireBullet(Entity* self, int keys, int bullet_delay, int* current_bullet_delay, Bullet bullet_array[], int bullet_array_len, int HitboxArray[][4], int HitboxLen) {
	if (*current_bullet_delay > 0) *current_bullet_delay = *current_bullet_delay - 1;
	else {
		if (keys & KEY_A || keys & KEY_B || keys & KEY_X || keys & KEY_Y) {
			// Finding bullet direction
			int x = 0;
			int y = 0;
			if (keys & KEY_A) x += 1;
			if (keys & KEY_Y) x -= 1;
			if (keys & KEY_B) y += 1;
			if (keys & KEY_X) y -= 1;

			float angle = GetAngleFromOriginTo(x, y);

			*current_bullet_delay = bullet_delay;  // Reset the bullet delay
			BulletSetupInBulletArray(
				bullet_array, MAXBULLETCOUNT,  // Bullet array information
				player_center[0] - 2, player_center[1] - 2,  // x, y
				5, 5,  // w, h
				angle,  // angle
				2,  // velocity
				120,  // lifespan
				1,  // damage
				PLAYERBULLET  // bullet type
			);
			EntityMoveAmount(&player, x * -1, y * -1, HitboxArray, HitboxLen);
		}
	}
}

void PlayerAnimate(Entity* self, int frame_number, int* player_animation_frame_number, u16* player_gfx_mem[], int current_bullet_delay) {
	if (frame_number % 6 == 0) {
			*player_animation_frame_number = *player_animation_frame_number + 1;
			*player_animation_frame_number = *player_animation_frame_number % 4;
	}
	if (current_bullet_delay == 0) {
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
			player_gfx_mem[4 + *player_animation_frame_number],
			-1,
			false,
			false,
			false,
			false,
			false
		);
	}
}

// Enemy Allocation, 15 total enemies, lets say 5 of each
Entity enemy_entity_array[3][5];
u16* EnemyGFXMem[3][8];

/*
Find difference in x and y compared to the player
Adjust vflip and xflip accordingly
Move accordingly
Attempt to shoot if close enough to the player or whenever possible

Vars
_Bool Vflip
_Bool Hflip
int bullet_delay
int current_bullet_delay
*/ 
typedef struct {
	_Bool v_flip;
	_Bool h_flip;
	int bullet_delay;
	int current_bullet_delay;
} BasicEnemy;

BasicEnemy basic_enemy_array[5];

void BasicEnemyHandle(BasicEnemy* basic_enemy_struct, Entity* self, int HitboxArray[][4], int HitboxLen) {
	int my_center[2];
	EntityGetCenterArray(self, my_center);

	int x_difference = player_center[0] - my_center[0];
	int y_difference = player_center[1] - my_center[1];

	_Bool move_along_x = 1;
	if (y_difference > x_difference) move_along_x = 0;

	if (x_difference > 0) basic_enemy_struct->h_flip = 0;
	else basic_enemy_struct->h_flip = 1;

	int y = 0;
	if (y_difference > 0) y = 1;
	else if (y_difference < 0) y = -1;

	EntityMoveAmount(self, 0, y, HitboxArray, HitboxLen);
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
	for (int a = 0; a < 8; a++) {
		PlayerGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + 16*16 * a, PlayerGFXMem[a], 16 * 16);
	}
	// Allocating memory for, and loading the player
	for (int a = 0; a < 4; a++) {
		EnemyGFXMem[0][a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + 16*16*8 + 16*16 * a, EnemyGFXMem[0][a], 16 * 16);
	}
	
	// Allocating memory for, and loading the player bullets
	for (int a = 0; a < 4; a++) {
		BulletGFXMem[0][a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + 16*16*8*7 + 16*16 * a, BulletGFXMem[0][a], 16 * 16);
	}
	// The above but for the basic enemy bullets
	for (int a = 0; a < 4; a++) {
		BulletGFXMem[1][a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + 16*16*8*6 + 16*16 * a, BulletGFXMem[1][a], 16 * 16);
	}
	
	// Setting up the player
	EntitySetup(
		&player,
		100, 100,  // X, Y
		13, 13,  // W, H
		1,  // Health, get one shot
		0   // Type
	);
	EntityGetRectArray(&player, player_hitbox);
	
	// Setting up the bullet array
	BulletInitBulletArray(bullet_array, MAXBULLETCOUNT);

	//
	EntitySetup(&enemy_entity_array[0][0], 25, 25, 16, 16, 10, 1);
	
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
		PlayerFireBullet(&player, keys, bullet_delay, &current_bullet_delay, bullet_array, MAXBULLETCOUNT, screen_boarder, 4);

		// Handle enemies here
		BasicEnemyHandle(&basic_enemy_array[0], &enemy_entity_array[0][0], screen_boarder, 4);
	
		// Kind of 'deleting' old bullets and then updating them if they go outside the screen
		BulletHandleBulletArray(bullet_array, MAXBULLETCOUNT, playable_area);

		// Bullet collision with player and enemies here
		
		// Counting Bullets, not necessary
		alive_bullets = 0;
		for (int i = 0; i < MAXBULLETCOUNT; i++) {
			if (bullet_array[i].alive) {
				alive_bullets++;
			}
		}
		
		// Drawing and animating the player
		PlayerAnimate(&player, frame_number, &player_animation_frame_number, PlayerGFXMem, current_bullet_delay);

		// Drawing and animating the enemy
		oamSet(
			&oamMain,
			5,
			enemy_entity_array[0][0].x, enemy_entity_array[0][0].y,
			0,
			0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			EnemyGFXMem[0][0],
			-1,
			false,
			false,
			false,
			false,
			false
		);
		
		// Drawing the bullets
		for (int i = 0; i < MAXBULLETCOUNT; i++) {
			oamSet(
				&oamMain,
				20 + i,
				bullet_array[i].x,
				bullet_array[i].y,
				0,
				0,
				SpriteSize_16x16,
				SpriteColorFormat_256Color,
				BulletGFXMem[bullet_array[i].type][bullet_array[i].lifespan / 6 % 4],
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

/*
Finished:
	Redo the player firing so it isnt dependent on movement and instead dependent on the ABXY keys
		Can fire diagonly as well, shouldnt be too hard future me

	Redo the player model as right now it feels like it is lacking colour
		Make it more boxy, less alien, maybe orange as well because the players bullets are orange, could be something like a construction drone aesthetic
*/