/*
ToDo:
	Explosions

	Redo the player animation so it loads the sprite every time it changes, this is mainly to conserve GFX
		If necessary
	
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
#define SENTINELBULLET 1
#define ENEYMINEBULLET 2

// A series of 2d arrays of pointers to the graphics memory of the sprites
u16* PlayerGFXMem[8];
u16* EnemyGFXMem[3][8];
u16* BulletGFXMem[4][4];

// Player Entity
Entity player;
int player_center[2];
int player_hitbox[4];

void PlayerMovement(Entity* self, int keys, int HitboxArray[][4], int HitboxLen) {
	// Finding the movement
	int x = 0;
	int y = 0;
	if (keys & KEY_RIGHT) x = 1;
	if (keys & KEY_LEFT) x = -1;
	if (keys & KEY_DOWN) y = 1;
	if (keys & KEY_UP) y = -1;

	EntityMove(self, x, y, HitboxArray, HitboxLen);
}

void PlayerFireBullet(Entity* self, int keys, Bullet bullet_array[], int bullet_array_len, int HitboxArray[][4], int HitboxLen) {
	if (self->current_bullet_delay > 0) self->current_bullet_delay -= 1;
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

			self->current_bullet_delay = self->bullet_delay;  // Reset the bullet delay
			BulletSetupInBulletArray(
				bullet_array, bullet_array_len,  // Bullet array information
				player_center[0] - 2, player_center[1] - 2,  // x, y
				5, 5,  // w, h
				angle,  // angle
				2,  // velocity
				120,  // lifespan
				1,  // damage
				PLAYERBULLET  // bullet type
			);

			// Player moves in the opposite direction
			EntityMove(&player, x * -1, y * -1, HitboxArray, HitboxLen);
		}
	}
}

void PlayerAnimate(Entity* self, int frame_number, u16* player_gfx_mem[]) {
	if (frame_number % 6 == 0) {
			self->animation_frame_number += 1;
			self->animation_frame_number %= 4;
	}
	oamSet(
			&oamMain,
			0,
			self->x, self->y,
			0,
			0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			player_gfx_mem[4 * (self->current_bullet_delay != 0) + self->animation_frame_number],
			-1,
			false,
			self->dead,
			self->v_flip,
			self->h_flip,
			false
		);
	/* Old animate code
	if (self->current_bullet_delay == 0) {
		oamSet(
			&oamMain,
			0,
			self->x, self->y,
			0,
			0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			player_gfx_mem[self->animation_frame_number],
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
			player_gfx_mem[4 + self->animation_frame_number],
			-1,
			false,
			false,
			false,
			false,
			false
		);
	}
	*/
}

// Enemy Allocation, 15 total enemies, lets say 5 of each
Entity enemy_entity_array[3][5];
int temp_enemy_hitbox[4];  // Used to hold an enemie's hitbox when handling said enemy
_Bool sentinel_move_direction_array[5];  // If false then the sentinel will only move along the x axis, y axis if true


/*
Find difference in x and y compared to the player
Adjust vflip and xflip accordingly
Move accordingly
Attempt to shoot if close enough to the player or whenever possible
Dont move is reloading
*/ 
/*
void BasicEnemyHandle(Entity* self, int HitboxArray[][4], int HitboxLen) {
	int my_center[2];
	EntityGetCenterArray(self, my_center);

	int x_difference = player_center[0] - my_center[0];
	int y_difference = player_center[1] - my_center[1];

	_Bool move_along_x = 1;
	if (y_difference > x_difference) move_along_x = 0;

	if (x_difference > 0) self->h_flip = 0;
	else self->h_flip = 1;

	int y = 0;
	if (y_difference > 0) y = 1;
	else if (y_difference < 0) y = -1;

	EntityMove(self, 0, y, HitboxArray, HitboxLen);
}
*/

void SentinelMove(Entity* self, _Bool sentinel_move_direction, int player_center[2], int HitboxArray[][4], int HitboxLen) {
	if (self->current_bullet_delay == 0) {
		int my_center[2];
		EntityGetCenterArray(self, my_center);
		
		int difference = player_center[sentinel_move_direction] - my_center[sentinel_move_direction];

		int movement = 0;

		if (difference > 0) movement = 1;
		else if (difference < 0) movement = -1;

		if (sentinel_move_direction == 0) {
			EntityMove(self, movement, 0, HitboxArray, HitboxLen);
			difference = player_center[!sentinel_move_direction] - my_center[!sentinel_move_direction];
			if (difference > 0) self->h_flip = 0;
			else self->h_flip = 1;	
		}
		else {
			EntityMove(self, 0, movement, HitboxArray, HitboxLen);
			difference = player_center[!sentinel_move_direction] - my_center[!sentinel_move_direction];
			if (difference > 0) self->v_flip = 0;
			else self->v_flip = 1;	
		}
	}
}

void SentinelFireBullet(Entity* self, _Bool sentinel_move_direction, Bullet bullet_array[], int bullet_array_len) {
	if (self->current_bullet_delay > 0) self->current_bullet_delay -= 1;
	else {
		int my_center[2];
		EntityGetCenterArray(self, my_center);
		
		int difference = player_center[sentinel_move_direction] - my_center[sentinel_move_direction];

		if (difference == 0) {

			difference = player_center[!sentinel_move_direction] - my_center[!sentinel_move_direction];

			float angle;

			if (sentinel_move_direction) {  // If firing horizontally
				if (difference > 0) angle = 0;
				else angle = PI;
			}
			else {
				if (difference > 0) angle = 3 * PI / 2;
				else angle = PI / 2;
			}

			self->current_bullet_delay = self->bullet_delay;
			BulletSetupInBulletArray(
				bullet_array, bullet_array_len,
				my_center[0] - 4, my_center[1] - 4,
				8, 8,
				angle,
				1,
				120,
				1,
				SENTINELBULLET
			);
		}
	}
}

void SentinelAnimate(Entity* self, int oam_number, int frame_number, u16* enemy_gfx_mem[8]) {
	if (frame_number % 6 == 0) {
			self->animation_frame_number += 1;
			self->animation_frame_number %= 4;
	}
	if (self->current_bullet_delay == 0) {
		oamSet(
			&oamMain,
			oam_number,
			self->x, self->y,
			0,
			0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			enemy_gfx_mem[self->animation_frame_number],
			-1,
			false,
			self->dead,
			self->v_flip,
			self->h_flip,
			false
		);
	}
	else {
		oamSet(
			&oamMain,
			oam_number,
			self->x, self->y,
			0,
			0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			enemy_gfx_mem[4 + 3 - self->current_bullet_delay / 15 % 4],
			-1,
			false,
			self->dead,
			self->v_flip,
			self->h_flip,
			false
		);
	}
	
}

// Take a guess
int frame_number = 0;

// Bullet array and other necessary information
Bullet bullet_array[MAXBULLETCOUNT];
int temp_bullet_hitbox[4];  // Used to hold a bullet's hitbox when handling said bullet

int alive_bullets = 0;

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
	// Allocating memory for, and loading all the sentinal sprites
	for (int a = 0; a < 8; a++) {
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
		0,   // Type
		10  // Bullet delay
	);
	EntityGetRectArray(&player, player_hitbox);
	
	// Setting up the bullet array
	BulletInitBulletArray(bullet_array, MAXBULLETCOUNT);

	// Basic sentinel stuff
	for (int i = 0; i < 5; i++) {
		enemy_entity_array[0][i].dead = 1;
	}

	EntitySetup(&enemy_entity_array[0][0], 15, 25, 16, 16, 10, 1, 60);
	sentinel_move_direction_array[0] = 1;
	enemy_entity_array[0][0].current_bullet_delay = 60;
	EntitySetup(&enemy_entity_array[0][1], 15, 25, 16, 16, 10, 1, 60);
	sentinel_move_direction_array[1] = 0;
	enemy_entity_array[0][1].current_bullet_delay = 60;

	EntitySetup(&enemy_entity_array[0][2], 225, 161, 16, 16, 10, 1, 60);
	sentinel_move_direction_array[2] = 1;
	enemy_entity_array[0][2].current_bullet_delay = 60;
	EntitySetup(&enemy_entity_array[0][3], 225, 161, 16, 16, 10, 1, 60);
	sentinel_move_direction_array[3] = 0;
	enemy_entity_array[0][3].current_bullet_delay = 60;
	
	
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
		
		// Player movement and bullet firing
		if (!player.dead) {
			PlayerMovement(&player, keys, screen_boarder, 4);

			// Collecting player infomation
			EntityGetRectArray(&player, player_hitbox);
			EntityGetCenterArray(&player, player_center);
			
			PlayerFireBullet(&player, keys, bullet_array, MAXBULLETCOUNT, screen_boarder, 4);
		}
		
		// Handle enemies here
		for (int i = 0; i < 5; i++) {
			if (!enemy_entity_array[0][i].dead) {
				SentinelMove(&enemy_entity_array[0][i], sentinel_move_direction_array[i], player_center, screen_boarder, 4);
				SentinelFireBullet(&enemy_entity_array[0][i], sentinel_move_direction_array[i], bullet_array, MAXBULLETCOUNT);
			}
		}
	
		// Kind of 'deleting' old bullets and then updating them if they go outside the screen
		BulletHandleBulletArray(bullet_array, MAXBULLETCOUNT, playable_area);

		// Bullet collision with player and enemies here
		for (int bullet_index = 0; bullet_index < MAXBULLETCOUNT; bullet_index++) {
			if (bullet_array[bullet_index].alive){
				BulletGetRectArray(&bullet_array[bullet_index], temp_bullet_hitbox);

				if (bullet_array[bullet_index].type == 0) {
					// Check against every alive enemy
					for (int a = 0; a < 3; a++) {
						for (int b = 0; b < 5; b++) {
							if (!enemy_entity_array[a][b].dead) {
								EntityGetRectArray(&enemy_entity_array[a][b], temp_enemy_hitbox);
								if (RectangleCollision(temp_bullet_hitbox, temp_enemy_hitbox)) {
									EntityTakeDamage(&enemy_entity_array[a][b], bullet_array[bullet_index].damage);
									bullet_array[bullet_index].to_die = 1;
								}
							}
						}
					}
				}
				else {
					// Check against player
					if (!player.dead) {
						if (RectangleCollision(temp_bullet_hitbox, player_hitbox)) {
							EntityTakeDamage(&player, bullet_array[bullet_index].damage);
							bullet_array[bullet_index].to_die = 1;
						}
					}
				}
			}
		}
		
		// Counting Bullets, not necessary
		alive_bullets = 0;
		for (int i = 0; i < MAXBULLETCOUNT; i++) {
			if (bullet_array[i].alive) {
				alive_bullets++;
			}
		}
		
		// Drawing and animating the player
		PlayerAnimate(&player, frame_number, PlayerGFXMem);

		// Draw enemies here
		for (int i = 0; i < 5; i++) {
			SentinelAnimate(&enemy_entity_array[0][i], 5 + i, frame_number, EnemyGFXMem[0]);
		}

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
				BulletGFXMem[bullet_array[i].type][3 - bullet_array[i].lifespan / 6 % 4],
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
		iprintf("Alive Bullets = %d\n", alive_bullets);
		iprintf("Current Bullet Delay = %d\n", player.current_bullet_delay);
		// iprintf("%d\n", keys);
		// iprintf("%d, %d, %d, %d\n", KEY_RIGHT, KEY_LEFT, KEY_UP, KEY_DOWN);
		// iprintf("%d, %d, %d, %d", keys & KEY_RIGHT, keys & KEY_LEFT, keys & KEY_UP, keys & KEY_DOWN);
		iprintf("Health = %d", enemy_entity_array[0][0].health);
		
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
		
	Improve player sprite so it isnt direction based
	
	Get the sentinel enemy working, animations, movement and all
*/