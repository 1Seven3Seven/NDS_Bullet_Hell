/*
ToDo:
	Level system / Portaling

	Be super evil and make the chaser shoot bullets

	Redo the player animation so it loads the sprite every time it changes, this is mainly to conserve GFX
		If necessary

	Redo all enemy animations so they load the new sprite
		Same as above
	
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
#define MINERMINE 2
#define MINERMINEBULLET 3
#define TILESIZE 256  // The number of pixels in the tiles
#define SPRITESHEETWIDTH 16  // Number of tiles the sprite sheet is wide

// A series of 2d arrays of pointers to the graphics memory of the sprites
u16* PlayerGFXMem[8];
u16* PlayerExplosionGFXMem[8];

u16* SentinelGFXMem[2][8];
u16* ChaserGFXMem[4];
u16* MinerGFXMem[8];
u16* ExplosionGFXMem[8];

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

void PlayerAnimate(Entity* self, int frame_number, u16* player_gfx_mem[], u16* player_explosion_gfx_mem[]) {
	if (!self->dead) {
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
			false,
			self->v_flip,
			self->h_flip,
			false
		);
	}
	else {
		if (self->counter > 0) self->counter -= 1;
		oamSet(
			&oamMain,
			0,
			self->x, self->y,
			0,
			0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			player_explosion_gfx_mem[7 - (int)(self->counter / 2)],
			-1,
			false,
			!self->counter,
			self->v_flip,
			self->h_flip,
			false
		);
	}
}

// Enemy Allocation, 15 total enemies, lets say up to 5 of each
Entity EnemyEntityArray[3][5];
int temp_enemy_hitbox[4];  // Used to hold an enemie's hitbox when handling said enemy

// Universal enemy stuff
void EnemySetupDeathAnimations(Entity enemy_entity_array[3][5]) {  // Should be called after seting up all the enemies for a stage
	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 5; b++) {
			if (!enemy_entity_array[a][b].dead) {
				enemy_entity_array[a][b].counter = 12;  // As the death animation lasts 12 frames
			}
		}
	}
}

// Sentinel handling
_Bool sentinel_move_direction_array[5];  // If false then the sentinel will only move along the x axis, y axis if true

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
				240,
				1,
				SENTINELBULLET
			);
		}
	}
}

void SentinelAnimate(Entity* self, int oam_number, int frame_number, u16* sentinel_gfx_mem[8], u16* sentinel_explosion_gfx_mem[8]) {
	if (!self->dead) {
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
				sentinel_gfx_mem[self->animation_frame_number],
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
				sentinel_gfx_mem[4 + 3 - self->current_bullet_delay / 15 % 4],
				-1,
				false,
				self->dead,
				self->v_flip,
				self->h_flip,
				false
			);
		}
	}
	else {
		if (self->counter > 0) self->counter -= 1;
		oamSet(
			&oamMain,
			oam_number,
			self->x, self->y,
			0,
			0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			sentinel_explosion_gfx_mem[7 - (int)(self->counter / 2)],
			-1,
			false,
			!self->counter,
			self->v_flip,
			self->h_flip,
			false
		);
	}
}

// Chaser handling
float chaser_movement_vector_array[5][2];

void ChaserMove(Entity* self, float vector[2], int player_center[2], int HitboxArray[][4], int HitboxLen) {
	if (self->bullet_delay == 0) {
		_Bool collision = 0;
		// Move in given direction
		collision = EntityMove(self, vector[0], vector[1], HitboxArray, HitboxLen);
		// Collision detection
		if (collision) {
			self->bullet_delay = 60;
		}
	}
	else {
		self->bullet_delay -= 1;
		if (self->bullet_delay == 0) {
			// Get vector to the player
			int my_center[2];
			EntityGetCenterArray(self, my_center);
			GetVectorFromAngle(GetAngleFromOriginTo(player_center[0] - my_center[0], player_center[1] - my_center[1]), vector);
			vector[0] *= 3;
			vector[1] *= 3;
		}
	}
}

void ChaserAnimate(Entity* self, int oam_number, int frame_number, u16* chaser_gfx_mem[4], u16* chaser_explosion_gfx_mem[8]) {
	if (!self->dead) {
		if (frame_number % 6 == 0) {
			self->animation_frame_number += 1;
			self->animation_frame_number %= 4;
		}
		oamSet(
			&oamMain,
			oam_number,
			self->x, self->y,
			0,
			0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			chaser_gfx_mem[self->animation_frame_number],
			-1,
			false,
			false,
			self->v_flip,
			self->h_flip,
			false
		);
	}
	else {
		if (self->counter > 0) self->counter -= 1;
		oamSet(
			&oamMain,
			oam_number,
			self->x, self->y,
			0,
			0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			chaser_explosion_gfx_mem[7 - (int)(self->counter / 2)],
			-1,
			false,
			!self->counter,
			self->v_flip,
			self->h_flip,
			false
		);
	}
}

// Miner handling
int miner_movement_vector_array[5][2];
int miner_place_mine_delay_array[5] = {30, 30, 30, 30, 30};

void MinerMove(Entity* self, int vector[2], int HitboxArray[][4], int HitboxLen) {
	_Bool x_movement = EntityMoveX(self, vector[0], HitboxArray, HitboxLen);
	if (x_movement) vector[0] = -vector[0];

	_Bool y_movement = EntityMoveY(self, vector[1], HitboxArray, HitboxLen);
	if (y_movement) vector[1] = -vector[1];
}

void MinerPlaceMine(Entity* self, int* miner_place_mine_delay ,Bullet bullet_array[], int bullet_array_len) {
	if (self->current_bullet_delay > 0) self->current_bullet_delay -= 1;
	else {
		if (*miner_place_mine_delay > 0) *miner_place_mine_delay -= 1;
		else {
			int my_center[2];
			EntityGetCenterArray(self, my_center);

			*miner_place_mine_delay = 30;
			self->current_bullet_delay = self->bullet_delay;
			BulletSetupInBulletArray(
				bullet_array, bullet_array_len,
				my_center[0] - 5, my_center[1] - 5,
				8, 8,
				0,
				0,
				240,
				1,
				MINERMINE
			);
		}
	}
}

void MinerAnimate(Entity* self, int oam_number, int frame_number, u16* miner_gfx_mem[], u16* miner_explosion_gfx_mem[]) {
	if (!self->dead) {
		if (frame_number % 6 == 0) {
			self->animation_frame_number += 1;
			self->animation_frame_number %= 4;
		}
		oamSet(
			&oamMain,
			oam_number,
			self->x, self->y,
			0,
			0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			miner_gfx_mem[4 * (self->current_bullet_delay != 0) + self->animation_frame_number],
			-1,
			false,
			false,
			self->v_flip,
			self->h_flip,
			false
		);
	}
	else {
		if (self->counter > 0) self->counter -= 1;
		oamSet(
			&oamMain,
			oam_number,
			self->x, self->y,
			0,
			0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			miner_explosion_gfx_mem[7 - (int)(self->counter / 2)],
			-1,
			false,
			!self->counter,
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
int temp_bullet_center[2];  // Used to hold a bullet's center when handling said bullet

int alive_bullets = 0;

// Main boarder hitboxes
int playable_area[4] = {0, 0, 256, 192};
int screen_boarder[4][4] = {
	{0, 0, 8, 192},
	{0, 0, 256, 8},
	{0, 184, 256, 8},
	{248, 0, 8, 192}
};

// Portal stuff - Screen size is 256 × 192 pixels - Middle is 128 x 96
int portal_hitboxes[4][4] = {
	{120, 40, 16, 16},    // Top
	{120, 136, 16, 16},  // Bottom
	{72, 88, 16, 16},   // Right
	{168, 88, 16, 16}  // Left
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
	
	// Allocating memory for, and loading the player sprites
	for (int a = 0; a < 8; a++) {
		PlayerGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + TILESIZE * a, PlayerGFXMem[a], 16 * 16);
	}
	// Allocating memory for, and loading the sentinal sprites
	for (int a = 0; a < 2; a++) {
		for (int b = 0; b < 8; b++) {
			SentinelGFXMem[a][b] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
			dmaCopy((u8*)SpriteSheetTiles + TILESIZE*SPRITESHEETWIDTH * (a + 1) + TILESIZE * b, SentinelGFXMem[a][b], 16 * 16);
		}
	}
	// Allocating memory for, and loading the chaser sprites
	for (int a = 0; a < 4; a++) {
		ChaserGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + TILESIZE * SPRITESHEETWIDTH * 3 + TILESIZE * a, ChaserGFXMem[a], 16 * 16);
	}
	// Allocating memory for, and loading the miner sprites
	for (int a = 0; a < 8; a++) {
		MinerGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + TILESIZE * SPRITESHEETWIDTH * 4 + TILESIZE * a, MinerGFXMem[a], 16 * 16);
	}

	// Allocating memory for, and loading the player explosion sprites
	for (int a = 0; a < 8; a++) {
		PlayerExplosionGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + TILESIZE * 8 +  TILESIZE * a, PlayerExplosionGFXMem[a], 16 * 16);
	}
	// Allocating memory for, and loading the explosion sprites
	for (int a = 0; a < 8; a++) {
		ExplosionGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + TILESIZE*SPRITESHEETWIDTH + TILESIZE * 8 + TILESIZE * a, ExplosionGFXMem[a], 16 * 16);
	}

	// Allocating memory for, and loading the player bullets
	for (int a = 0; a < 4; a++) {
		BulletGFXMem[0][a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + TILESIZE*SPRITESHEETWIDTH*7 + TILESIZE * a, BulletGFXMem[0][a], 16 * 16);
	}
	// The above but for the sentinel bullets
	for (int a = 0; a < 4; a++) {
		BulletGFXMem[1][a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + TILESIZE*SPRITESHEETWIDTH*6 + TILESIZE * a, BulletGFXMem[1][a], 16 * 16);
	}
	// The above but for the mines
	for (int a = 0; a < 4; a++) {
		BulletGFXMem[2][a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + TILESIZE*SPRITESHEETWIDTH*5 + TILESIZE * a, BulletGFXMem[2][a], 16 * 16);
	}
	// The above but for the mines bullets
	for (int a = 0; a < 4; a++) {
		BulletGFXMem[3][a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + TILESIZE*SPRITESHEETWIDTH*5 + TILESIZE * 4 + TILESIZE * a, BulletGFXMem[3][a], 16 * 16);
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
	player.counter = 16;
	
	// Setting up the bullet array
	BulletInitBulletArray(bullet_array, MAXBULLETCOUNT);

	// Every enemy needs to start off as dead
	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 5; b++) {
			EnemyEntityArray[a][b].dead = 1;
		}
	}

	/* Sentinel example
	// Do one for easy, both for hard
	EntitySetup(&EnemyEntityArray[0][0], 15, 15, 16, 16, 10, 1, 60);
	sentinel_move_direction_array[0] = 1;
	EnemyEntityArray[0][0].current_bullet_delay = 60;
	EntitySetup(&EnemyEntityArray[0][1], 15, 15, 16, 16, 10, 1, 60);
	sentinel_move_direction_array[1] = 0;
	EnemyEntityArray[0][1].current_bullet_delay = 60;

	EntitySetup(&EnemyEntityArray[0][2], 225, 161, 16, 16, 10, 1, 60);
	sentinel_move_direction_array[2] = 1;
	EnemyEntityArray[0][2].current_bullet_delay = 60;
	EntitySetup(&EnemyEntityArray[0][3], 225, 161, 16, 16, 10, 1, 60);
	sentinel_move_direction_array[3] = 0;
	EnemyEntityArray[0][3].current_bullet_delay = 60;
	*/

	/* Chaser example
	// Easy
	EntitySetup(&EnemyEntityArray[1][0], 15, 15, 15, 15, 3, 2, 60);
	EntitySetup(&EnemyEntityArray[1][1], 15, 65, 15, 15, 3, 2, 60);
	EntitySetup(&EnemyEntityArray[1][2], 15, 90, 15, 15, 3, 2, 60);
	EntitySetup(&EnemyEntityArray[1][3], 15, 111, 15, 15, 3, 2, 60);
	EntitySetup(&EnemyEntityArray[1][4], 15, 161, 15, 15, 3, 2, 60);

	// Hard
	EntitySetup(&EnemyEntityArray[1][0], 15, 15, 15, 15, 3, 2, 60);
	EntitySetup(&EnemyEntityArray[1][1], 225, 15, 15, 15, 3, 2, 60);
	EntitySetup(&EnemyEntityArray[1][2], 15, 161, 15, 15, 3, 2, 60);
	EntitySetup(&EnemyEntityArray[1][3], 226, 161, 15, 15, 3, 2, 60);
	*/

	/* Miner example
	// Do all for hard, less for easy
	EntitySetup(&EnemyEntityArray[2][0], 15, 15, 16, 16, 5, 3, 120);
	EnemyEntityArray[2][0].current_bullet_delay = 10;
	miner_movement_vector_array[0][0] = 1;
	miner_movement_vector_array[0][1] = 1;
	EntitySetup(&EnemyEntityArray[2][1], 15, 65, 16, 16, 5, 3, 120);
	EnemyEntityArray[2][0].current_bullet_delay = 10;
	miner_movement_vector_array[1][0] = 1;
	miner_movement_vector_array[1][1] = 1;
	EntitySetup(&EnemyEntityArray[2][2], 65, 15, 16, 16, 5, 3, 120);
	EnemyEntityArray[2][0].current_bullet_delay = 10;
	miner_movement_vector_array[2][0] = 1;
	miner_movement_vector_array[2][1] = 1;
	EntitySetup(&EnemyEntityArray[2][3], 65, 65, 16, 16, 5, 3, 120);
	EnemyEntityArray[2][3].current_bullet_delay = 10;
	miner_movement_vector_array[3][0] = -1;
	miner_movement_vector_array[3][1] = -1;
	*/

	EntitySetup(&EnemyEntityArray[2][0], 15, 15, 16, 16, 5, 3, 120);
	EnemyEntityArray[2][0].current_bullet_delay = 10;
	miner_movement_vector_array[0][0] = 1;
	miner_movement_vector_array[0][1] = 1;
	EntitySetup(&EnemyEntityArray[2][1], 15, 65, 16, 16, 5, 3, 120);
	EnemyEntityArray[2][0].current_bullet_delay = 10;
	miner_movement_vector_array[1][0] = 1;
	miner_movement_vector_array[1][1] = 1;
	EntitySetup(&EnemyEntityArray[2][2], 65, 15, 16, 16, 5, 3, 120);
	EnemyEntityArray[2][0].current_bullet_delay = 10;
	miner_movement_vector_array[2][0] = 1;
	miner_movement_vector_array[2][1] = 1;
	EntitySetup(&EnemyEntityArray[2][3], 65, 65, 16, 16, 5, 3, 120);
	EnemyEntityArray[2][3].current_bullet_delay = 10;
	miner_movement_vector_array[3][0] = -1;
	miner_movement_vector_array[3][1] = -1;

	EnemySetupDeathAnimations(EnemyEntityArray);
	
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
			// Sentinel handling
			if (!EnemyEntityArray[0][i].dead) {
				SentinelMove(&EnemyEntityArray[0][i], sentinel_move_direction_array[i], player_center, screen_boarder, 4);
				SentinelFireBullet(&EnemyEntityArray[0][i], sentinel_move_direction_array[i], bullet_array, MAXBULLETCOUNT);
			}

			// Chaser handling
			if (!EnemyEntityArray[1][i].dead) {
				ChaserMove(&EnemyEntityArray[1][i], chaser_movement_vector_array[i], player_center, screen_boarder, 4);
			}

			// Miner handling
			if (!EnemyEntityArray[2][i].dead) {
				MinerMove(&EnemyEntityArray[2][i], miner_movement_vector_array[i], screen_boarder, 4);
				MinerPlaceMine(&EnemyEntityArray[2][i], &miner_place_mine_delay_array[i], bullet_array, MAXBULLETCOUNT);
			}

		}
	
		// Kind of 'deleting' old bullets and then updating them if they go outside the screen
		BulletHandleBulletArray(bullet_array, MAXBULLETCOUNT, playable_area);
		// Spawning in the mines offspring
		for (int i = 0; i < MAXBULLETCOUNT; i++) {
			if (bullet_array[i].type == MINERMINE) {
				if (bullet_array[i].to_die) {
					BulletGetCenterArray(&bullet_array[i], temp_bullet_center);
					for (int a = 0; a < 8; a++) {
						BulletSetupInBulletArray(
							bullet_array,
							MAXBULLETCOUNT,
							temp_bullet_center[0] - 1, temp_bullet_center[1] - 1,
							3, 3,
							PI / 4 * a,
							1,
							60,
							1,
							MINERMINEBULLET
						);
					}
				}
			}
		}

		// Bullet collision with player and enemies
		for (int bullet_index = 0; bullet_index < MAXBULLETCOUNT; bullet_index++) {
			if (bullet_array[bullet_index].alive){
				BulletGetRectArray(&bullet_array[bullet_index], temp_bullet_hitbox);

				if (bullet_array[bullet_index].type == 0) {
					// Check against every alive enemy
					for (int a = 0; a < 3; a++) {
						for (int b = 0; b < 5; b++) {
							if (!EnemyEntityArray[a][b].dead) {
								EntityGetRectArray(&EnemyEntityArray[a][b], temp_enemy_hitbox);
								if (RectangleCollision(temp_bullet_hitbox, temp_enemy_hitbox)) {
									EntityTakeDamage(&EnemyEntityArray[a][b], bullet_array[bullet_index].damage);
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
		
		// Player collision with enemies
		for (int a = 0; a < 3; a++) {
			for (int b = 0; b < 5; b++) {
				if (!EnemyEntityArray[a][b].dead) {
					EntityGetRectArray(&EnemyEntityArray[a][b], temp_enemy_hitbox);
					if (RectangleCollision(player_hitbox, temp_enemy_hitbox)) {
						EntityTakeDamage(&player, 1);
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
		PlayerAnimate(&player, frame_number, PlayerGFXMem, PlayerExplosionGFXMem);

		// Draw enemies here
		for (int i = 0; i < 5; i++) {
			SentinelAnimate(&EnemyEntityArray[0][i], 5 + i, frame_number, SentinelGFXMem[!sentinel_move_direction_array[i]], ExplosionGFXMem);
			ChaserAnimate(&EnemyEntityArray[1][i], 10 + i, frame_number, ChaserGFXMem, ExplosionGFXMem);
			MinerAnimate(&EnemyEntityArray[2][i], 15 + i, frame_number, MinerGFXMem, ExplosionGFXMem);
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
		iprintf("Player Dead = %d\n", player.dead);
		for (int a = 0; a < 3; a++) {
			for (int b = 0; b < 5; b++) {
				iprintf("%d", EnemyEntityArray[a][b].dead);
			}
		}
		
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
	
	Explosions
		This means explosion animations

	Sawblade enemy
		Rush at player
		Go untill hit either the player or the boarder
		Wait a bit, then repeat
		Dont forget their animations

	Mine layer enemy
		This includes movement, which should be easy
		The mines, and their explosions
			Yes i want the mines themselves to explode
*/