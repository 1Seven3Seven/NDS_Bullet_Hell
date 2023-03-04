/*
ToDo:
	Level system / Portaling <-- DO THIS FUTURE ME

	Improve the miner as it doesnt feel right in comparison to the other enemies
		Either sprite wise or ai wise
			Duncan noted that it is the only one that doesnt have movement based off of the player, maybe that has something to do with it

	Redo the player animation so it loads the sprite every time it changes, this is mainly to conserve GFX
		If necessary

	Redo all enemy animations so they load the new sprite

	Potentially do some more research into sounds
		They make everything better
		https://maxmod.devkitpro.org/ref/tut/dsprog.html
		https://gbatemp.net/threads/maxmod-hates-me.327662/
		Google search "libnds No rule to make target, needed by 'soundbank.bin'."
		C:\devkitPro\examples\nds\audio\maxmod\basic_sound

	Semi-persistent laser as an idea for a miniboss attack
	Super Sentinel as another idea for a mini boss
*/

// Default stuff
#include <nds.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>	// Needed for the random number seeding - Mr. King

// My stuff
#include "GameLib.h"

// Pictures and sprites
#include "SpriteSheet.h"
#include "BasicBackground.h"

// Constants
#define MAXBULLETCOUNT 110
#define PI 3.14159265359
#define TILESIZE 256  // The number of pixels in the tiles
#define SPRITESHEETWIDTH 16  // Number of tiles the sprite sheet is wide


//---------------------------------------------------------------------------------
// A series of 2d arrays of pointers to the graphics memory of the sprites
//---------------------------------------------------------------------------------
u16* PlayerGFXMem[8];
u16* PlayerExplosionGFXMem[8];

u16* SentinelGFXMem[2][8];
u16* ShredderGFXMem[4];
u16* MinerGFXMem[8];
u16* ExplosionGFXMem[8];

u16* BulletGFXMem[4][4];


//---------------------------------------------------------------------------------
// Bullet array and other necessary information
//---------------------------------------------------------------------------------
#define PLAYERBULLET 0
#define SENTINELBULLET 1
#define MINERMINE 2
#define MINERMINEBULLET 3

Bullet BulletArray[MAXBULLETCOUNT];
int TempBulletHitbox[4];  // Used to hold a bullets hitbox when handling said bullet
int TempBulletCenter[2];  // Used to hold a bullets center when handling said bullet

int NumberOfAliveBullets = 0;


//---------------------------------------------------------------------------------
// Player Allocation and handling
//---------------------------------------------------------------------------------
#define PLAYERSTARTX 100
#define PLAYERSTARTY 100

Entity Player;
int PlayerCenter[2];
int PlayerHitbox[4];

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
	if (self->current_bullet_delay > 0) self->current_bullet_delay--;
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
				PlayerCenter[0] - 2, PlayerCenter[1] - 2,  // x, y
				5, 5,  // w, h
				angle,  // angle
				2,  // velocity
				120,  // lifespan
				1,  // damage
				PLAYERBULLET  // bullet type
			);

			// Player moves in the opposite direction
			EntityMove(&Player, x * -1, y * -1, HitboxArray, HitboxLen);
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
		if (self->counter > 0) self->counter--;
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

void PlayerQuickSetup(Entity* player) {
	EntitySetup(
			player,
			PLAYERSTARTX, PLAYERSTARTY,  // X, Y
			13, 13,                     // W, H
			1,                         // Health, get one shot
			0,                        // Type
			10                       // Bullet delay
		);
	player->counter = 16;
}


//---------------------------------------------------------------------------------
// Enemy Allocation and handling, 8 total enemies, max of 4 of one type
//---------------------------------------------------------------------------------
Entity EnemyEntityArray[8];
int TempEnemyHitbox[4];  // Used to hold an enemies hitbox when handling said enemy
int TempEnemyCenter[2];  // Used to hold an enemies center when handling said enemy

// Enemy setup stuff
void EnemyDeaden() {  // Should be called before setting up all the enemies for a stage
	for (int i = 0; i < 8; i++) {
		EnemyEntityArray[i].dead = 1;
		EnemyEntityArray[i].counter = 0;
	}
}

void EnemySetupDeathAnimations() {  // Should be called after seting up all the enemies for a stage
	for (int i = 0; i < 8; i++) {
		if (!EnemyEntityArray[i].dead) {
			EnemyEntityArray[i].counter = 12;  // As the death animation lasts 12 frames
		}
	}
}

// Sentinel handling
#define SENTINELHEALTH 10
#define SENTINELTYPE 1
#define SENTINELBULLETDELAY 60
#define SENTINELSTARTDELAY 60

_Bool SentinelMoveDirectionArray[4];  // If false then the sentinel will only move along the x axis, y axis if true

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
	if (self->current_bullet_delay > 0) self->current_bullet_delay--;
	else {
		int my_center[2];
		EntityGetCenterArray(self, my_center);
		
		int difference = PlayerCenter[sentinel_move_direction] - my_center[sentinel_move_direction];

		if (difference == 0) {

			difference = PlayerCenter[!sentinel_move_direction] - my_center[!sentinel_move_direction];

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

// Shredder handling
#define SHREDDERHEALTH 3
#define SHREDDERTYPE 2
#define SHREDDERBULLETDELAY 60
#define SHREDDERSTARTDELAY 60
#define SHREDDERSPEED 3

float ShredderMovementVectorArray[4][2];

void ShredderMove(Entity* self, float vector[2], int player_center[2], int HitboxArray[][4], int HitboxLen) {
	if (self->current_bullet_delay == 0) {
		_Bool collision = 0;
		// Move in given direction
		collision = EntityMove(self, vector[0], vector[1], HitboxArray, HitboxLen);
		// Collision detection to make them pause when at walls
		if (collision) {
			self->current_bullet_delay = self->bullet_delay;
		}
	}
	else {
		self->current_bullet_delay--;
		if (self->current_bullet_delay == 0) {
			// Get vector to the player
			int my_center[2];
			EntityGetCenterArray(self, my_center);
			GetVectorFromAngle(GetAngleFromOriginTo(player_center[0] - my_center[0], player_center[1] - my_center[1]), vector);
			vector[0] *= SHREDDERSPEED;
			vector[1] *= SHREDDERSPEED;
		}
	}
}

void ShredderAnimate(Entity* self, int oam_number, int frame_number, u16* shredder_gfx_mem[4], u16* shredder_explosion_gfx_mem[8]) {
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
			shredder_gfx_mem[self->animation_frame_number],
			-1,
			false,
			false,
			self->v_flip,
			self->h_flip,
			false
		);
	}
	else {
		if (self->counter > 0) self->counter--;
		oamSet(
			&oamMain,
			oam_number,
			self->x, self->y,
			0,
			0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			shredder_explosion_gfx_mem[7 - (int)(self->counter / 2)],
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
#define MINERHEALTH 5
#define MINERTYPE 3
#define MINERBULLETDELAY 120
#define MINERSTARTDELAY 60
#define MINERPLACEMINEDELAY 30

float MinerMovementVectorArray[4][2];
int MinerPlaceMineDelayArray[4] = {30, 30, 30, 30};

void MinerMove(Entity* self, float vector[2], int HitboxArray[][4], int HitboxLen) {
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

			*miner_place_mine_delay = MINERPLACEMINEDELAY;
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
		if (self->counter > 0) self->counter--;
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


//---------------------------------------------------------------------------------
// Enemy setup
//---------------------------------------------------------------------------------
int NumActiveSentinels = 0;
int NumActiveShredders = 0;
int NumActiveMiners = 0;

void EnemyResetCounters() {
	NumActiveSentinels = 0;
	NumActiveShredders = 0;
	NumActiveMiners = 0;
}

_Bool EnemySetupSentinel(int x, int y, _Bool move_direction) {
	_Bool success = 0;

	if (NumActiveSentinels < 4) {

		int index = EntitySetupInEntityArray(
			EnemyEntityArray, 8,
			x, y, 16, 16, SENTINELHEALTH, SENTINELTYPE, SENTINELBULLETDELAY
		);

		if (index >= 0) {
			SentinelMoveDirectionArray[index] = move_direction;
			EnemyEntityArray[index].current_bullet_delay = SENTINELSTARTDELAY;

			NumActiveSentinels++;
			success = 1;
		}
		
	}

	return success;
}

_Bool EnemySetupShredder(int x, int y) {
	_Bool success = 0;

	if (NumActiveShredders < 4) {
		int index = EntitySetupInEntityArray(
			EnemyEntityArray, 8,
			x, y, 15, 15, SHREDDERHEALTH, SHREDDERTYPE, SHREDDERBULLETDELAY
		);

		if (index >= 0) {
			EnemyEntityArray[index].current_bullet_delay = SHREDDERSTARTDELAY;

			NumActiveShredders++;
			success = 1;
		}
	}

	return success;
}

_Bool EnemySetupMiner(int x, int y, float vx, float vy) {
	_Bool success = 0;

	if (NumActiveMiners < 4) {
		int index = EntitySetupInEntityArray(
			EnemyEntityArray, 8,
			x, y, 16, 16, MINERHEALTH, MINERTYPE, MINERBULLETDELAY
		);

		if (index >= 0) {
			EnemyEntityArray[index].current_bullet_delay = MINERSTARTDELAY;
			MinerMovementVectorArray[index][0] = vx;
			MinerMovementVectorArray[index][1] = vy;

			NumActiveMiners++;
			success = 1;
		}
	}

	return success;

}


//---------------------------------------------------------------------------------
// Fast enemy loading
// We have 3 difficulties
// Each difficulty has some presets
// Difficulty 1 has only one type of enemy, and only up to two of them
// Difficulty 2 has up to two tyes of enemies, up to four in total
// Difficulty 3 has up to three types of enemies, up to siz in total
//---------------------------------------------------------------------------------
int Difficulty1Presets[3][4][4] = {
//                     ^ Enemy index
//                        ^ Number of enemies to chose from, here there are 4 enemies, but only 2 will be chosen
//                           ^ Enemy data [x, y, move_direction or vx, vy] vx and vy are divided by 10
	{  // Sentinels, x, y, move_direction, NONE
		{31, 15, 0, 0},
		{15, 31, 1, 0},
		{209, 161, 0, 0},
		{225, 145, 1, 0}
	},
	{  // Shredders, x, y, NONE, NONE
		{15, 15, 0, 0},
		{15, 162, 0, 0},
		{226, 15, 0, 0},
		{226, 162, 0, 0}
	},
	{  // Miners, x, y, vx, vy - Middle is 128 x 96
		{112, 15, 5, -5},
		{112, 161, -5, 5},
		{15, 80, -5, 5},
		{225, 80, 5, -5}
	}
};

int Difficulty1EnemyIndexes[4] = {0, 1, 2, 3};  // As there are four different enemy presets to chose from

void LoadDifficulty1Sentinels(int difficulty_1_indexes[4]) {
	EnemySetupSentinel(Difficulty1Presets[0][difficulty_1_indexes[0]][0], Difficulty1Presets[0][difficulty_1_indexes[0]][1], Difficulty1Presets[0][difficulty_1_indexes[0]][2]);
	EnemySetupSentinel(Difficulty1Presets[0][difficulty_1_indexes[1]][0], Difficulty1Presets[0][difficulty_1_indexes[1]][1], Difficulty1Presets[0][difficulty_1_indexes[1]][2]);
}

void LoadDifficulty1Shredders(int difficulty_1_indexes[4]) {
	EnemySetupShredder(Difficulty1Presets[1][difficulty_1_indexes[0]][0], Difficulty1Presets[1][difficulty_1_indexes[0]][1]);
	EnemySetupShredder(Difficulty1Presets[1][difficulty_1_indexes[1]][0], Difficulty1Presets[1][difficulty_1_indexes[1]][1]);
}

void LoadDifficulty1Miners(int difficulty_1_indexes[4]) {
	EnemySetupMiner(Difficulty1Presets[2][difficulty_1_indexes[0]][0], Difficulty1Presets[2][difficulty_1_indexes[0]][1], (float)Difficulty1Presets[2][difficulty_1_indexes[0]][2] / 10, (float)Difficulty1Presets[2][difficulty_1_indexes[0]][3] / 10);
	EnemySetupMiner(Difficulty1Presets[2][difficulty_1_indexes[1]][0], Difficulty1Presets[2][difficulty_1_indexes[1]][1], (float)Difficulty1Presets[2][difficulty_1_indexes[1]][2] / 10, (float)Difficulty1Presets[2][difficulty_1_indexes[1]][3] / 10);
}

void LoadRandomDifficulty1Enemies() {

	EnemyDeaden();

	int difficulty_1_indexes[4] = {0, 1, 2, 3};

	EnemyResetCounters();

	Shuffle(difficulty_1_indexes, 4);

	int index = rand() % 3;

	switch (index)
	{
	case 0:
		LoadDifficulty1Sentinels(difficulty_1_indexes);
		break;

	case 1:
		LoadDifficulty1Shredders(difficulty_1_indexes);
		break;

	case 2:
		LoadDifficulty1Miners(difficulty_1_indexes);
		break;
	}
	
	EnemySetupDeathAnimations();
}


int Difficulty2Presets[3][4][4] = {
//                     ^ Enemy index
//                        ^ Number of enemies to chose from, here there are 4 enemies, but only 2 will be chosen
//                           ^ Enemy data [x, y, move_direction or vx, vy] vx and vy are divided by 10
	{  // Sentinels, x, y, move_direction, NONE
		{31, 15, 0, 0},
		{15, 31, 1, 0},
		{209, 161, 0, 0},
		{225, 145, 1, 0}
	},
	{  // Shredders, x, y, NONE, NONE
		{15, 15, 0, 0},
		{15, 162, 0, 0},
		{226, 15, 0, 0},
		{226, 162, 0, 0}
	},
	{  // Miners, x, y, vx, vy - Middle is 128 x 96
		{112, 15, 5, -5},
		{112, 161, -5, 5},
		{15, 80, -5, 5},
		{225, 80, 5, -5}
	}
};

void LoadRandomDifficulty2Enemies() {}


int Difficulty3Enemies[3][4][4];

void LoadRandomDifficulty3Enemies() {}


//---------------------------------------------------------------------------------
// Map and map movement
// Upon all enemies being defeated and all enemy bullets decaying, portal to the next sector
// If the player dies, re-setup the enemies and player
//---------------------------------------------------------------------------------

int Level = 0;  // The current difficulty level, 0 = no change, 1 = one bullet spawned on death, 2 = three bullets spawned on death, mini bosses for levels 1 and 2, final boss for 3
int Sector = 0;  // The current sector, seven sectors for each level, in pairs if difficulties 1, 2 and 3 and a final boss


//---------------------------------------------------------------------------------
// Miscellaneous
//---------------------------------------------------------------------------------
int FrameNumber = 0;  // Take a guess

// Main boarder hitboxes
int PlayableArea[4] = {0, 0, 256, 192};
int ScreenBoarder[4][4] = {
	{0, 0, 8, 192},
	{0, 0, 256, 8},
	{0, 184, 256, 8},
	{248, 0, 8, 192}
};


//---------------------------------------------------------------------------------
// Easy handling and drawing of the player, enemies and bullets
//---------------------------------------------------------------------------------

void PlayerHandle(int keys) {
	if (!Player.dead) {
		PlayerMovement(&Player, keys, ScreenBoarder, 4);

		// Collecting player infomation
		EntityGetRectArray(&Player, PlayerHitbox);
		EntityGetCenterArray(&Player, PlayerCenter);
		
		PlayerFireBullet(&Player, keys, BulletArray, MAXBULLETCOUNT, ScreenBoarder, 4);
	}
}

void PlayerCheckCollisionAgainstAllEnemies() {
	for (int i = 0; i < 8; i++) {
		if (!EnemyEntityArray[i].dead) {
			EntityGetRectArray(&EnemyEntityArray[i], TempEnemyHitbox);
			if (RectangleCollision(PlayerHitbox, TempEnemyHitbox)) {
				EntityTakeDamage(&Player, 1);
			}
		}
	}
}

void EnemyHandleAll(int player_center[2], int HitboxArray[][4], int HitboxArrayLen) {
	int sentinel_number = 0;
	int shredder_number = 0;
	int miner_number = 0;

	for (int i = 0; i < 8; i++) {
		switch(EnemyEntityArray[i].type) {
			case SENTINELTYPE:
				if (!EnemyEntityArray[i].dead) {
					SentinelMove(&EnemyEntityArray[i], SentinelMoveDirectionArray[sentinel_number], player_center, HitboxArray, HitboxArrayLen);
					SentinelFireBullet(&EnemyEntityArray[i], SentinelMoveDirectionArray[sentinel_number], BulletArray, MAXBULLETCOUNT);
				}
				sentinel_number++;
				break;

			case SHREDDERTYPE:
				if (!EnemyEntityArray[i].dead) {
					ShredderMove(&EnemyEntityArray[i], ShredderMovementVectorArray[shredder_number], player_center, HitboxArray, HitboxArrayLen);
				}
				shredder_number++;
				break;

			case MINERTYPE:
				if (!EnemyEntityArray[i].dead) {
					MinerMove(&EnemyEntityArray[i], MinerMovementVectorArray[miner_number], HitboxArray, HitboxArrayLen);
					MinerPlaceMine(&EnemyEntityArray[i], &MinerPlaceMineDelayArray[miner_number], BulletArray, MAXBULLETCOUNT);
				}
				miner_number++;
				break;
		}
	}
}

void EnemyDrawAll(int frame_number) {
	int sentinel_number = 0;

	for (int i = 0; i < 8; i++) {
		switch(EnemyEntityArray[i].type) {
			case SENTINELTYPE:
				SentinelAnimate(&EnemyEntityArray[i], i + 1, frame_number, SentinelGFXMem[!SentinelMoveDirectionArray[sentinel_number]], ExplosionGFXMem);
				sentinel_number++;
				break;

			case SHREDDERTYPE:
				ShredderAnimate(&EnemyEntityArray[i], i + 1, frame_number, ShredderGFXMem, ExplosionGFXMem);
				break;

			case MINERTYPE:
				MinerAnimate(&EnemyEntityArray[i], i + 1, frame_number, MinerGFXMem, ExplosionGFXMem);
				break;
		}
	}
}

void BulletSpawnMineOffspring() {
	for (int i = 0; i < MAXBULLETCOUNT; i++) {
		if (BulletArray[i].type == MINERMINE) {
			if (BulletArray[i].to_die) {
				BulletGetCenterArray(&BulletArray[i], TempBulletCenter);
				for (int a = 0; a < 8; a++) {
					BulletSetupInBulletArray(
						BulletArray,
						MAXBULLETCOUNT,
						TempBulletCenter[0] - 1, TempBulletCenter[1] - 1,
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
}

void BulletCollisionWithPlayerAndEnemies() {
	for (int bullet_index = 0; bullet_index < MAXBULLETCOUNT; bullet_index++) {
		if (BulletArray[bullet_index].alive){
			BulletGetRectArray(&BulletArray[bullet_index], TempBulletHitbox);

			if (BulletArray[bullet_index].type == 0) {
				// Check against every alive enemy
				for (int i = 0; i < 8; i++) {
					if (!EnemyEntityArray[i].dead) {
						EntityGetRectArray(&EnemyEntityArray[i], TempEnemyHitbox);
						if (RectangleCollision(TempBulletHitbox, TempEnemyHitbox)) {
							EntityTakeDamage(&EnemyEntityArray[i], BulletArray[bullet_index].damage);
							BulletArray[bullet_index].to_die = 1;
						}
					}
				}
			}
			else {
				// Check against player
				if (!Player.dead) {
					if (RectangleCollision(TempBulletHitbox, PlayerHitbox)) {
						EntityTakeDamage(&Player, BulletArray[bullet_index].damage);
						BulletArray[bullet_index].to_die = 1;
					}
				}
			}
		}
	}
}

void BulletCountAlive() {
	NumberOfAliveBullets = 0;
	for (int i = 0; i < MAXBULLETCOUNT; i++) {
		if (BulletArray[i].alive) {
			NumberOfAliveBullets++;
		}
	}
}

void BulletDrawAll() {
	for (int i = 0; i < MAXBULLETCOUNT; i++) {
		oamSet(
			&oamMain,
			18 + i,
			BulletArray[i].x,
			BulletArray[i].y,
			0,
			0,
			SpriteSize_16x16,
			SpriteColorFormat_256Color,
			BulletGFXMem[BulletArray[i].type][3 - BulletArray[i].lifespan / 6 % 4],
			-1,
			false,
			!BulletArray[i].alive,
			false,
			false,
			false
		);
	}
}

//---------------------------------------------------------------------------------
// Combat loops
// Player portals in
// Enemies all portal in at the same time
// Pause, comnat starts
// Once all enemies are dead and all bullets decayed
// 		Player portals away
//		Pause, change background, repeat
//---------------------------------------------------------------------------------


//---------------------------------------------------------------------------------
void level_1_enemy_combat_loop(int bg) {
//---------------------------------------------------------------------------------

	/* Order of doing things
	Change the background
	Setup the player
	Setup the bullet array
	Setup the enemies

	LOOP
	*/

	// Loading new background
	// dmaCopy(BasicBackgroundBitmap, bgGetGfxPtr(bg), 256*256);
	// dmaCopy(BasicBackgroundPal, BG_PALETTE, sizeof(BasicBackgroundPal));


	// Player setup
	PlayerQuickSetup(&Player);

	// Bullet array setup
	BulletInitBulletArray(BulletArray, MAXBULLETCOUNT);

	// Setting up the enemies depending on the sector
	switch ((int)(Sector / 2)) {
		case 0:
			LoadRandomDifficulty1Enemies();
			break;

		case 1:
			LoadRandomDifficulty2Enemies();
			break;

		case 2:
			LoadRandomDifficulty3Enemies();
			break;
	}

	while (1) {
		// Clear the text
		consoleClear();
		// Get key presses
		scanKeys();
		// int keys = keysHeld();
		// int pressed = keysDown();
		// Frame number
		FrameNumber++;
		FrameNumber %= 60;

		// Player

		// Enemies

		// Bullets

		// Collision

		// Drawing

		// Text display

	}

}


//---------------------------------------------------------------------------------
void level_2_enemy_combat_loop() {
//---------------------------------------------------------------------------------

	// here

}


//---------------------------------------------------------------------------------
void level_3_enemy_combat_loop() {
//---------------------------------------------------------------------------------

	// here

}


//---------------------------------------------------------------------------------
void mini_boss_1_loop() {
//---------------------------------------------------------------------------------

	// here

}


//---------------------------------------------------------------------------------
void mini_boss_2_loop() {
//---------------------------------------------------------------------------------

	// here

}


//---------------------------------------------------------------------------------
void final_boss_loop() {
//---------------------------------------------------------------------------------

	// here

}


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

	// Seeding the random number generator
	time_t t;
	srand((unsigned) time(&t));
	
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
	// Allocating memory for, and loading the shredder sprites
	for (int a = 0; a < 4; a++) {
		ShredderGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + TILESIZE * SPRITESHEETWIDTH * 3 + TILESIZE * a, ShredderGFXMem[a], 16 * 16);
	}
	// Allocating memory for, and loading the miner sprites
	for (int a = 0; a < 8; a++) {
		MinerGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + TILESIZE * SPRITESHEETWIDTH * 4 + TILESIZE * a, MinerGFXMem[a], 16 * 16);
	}

	// Allocating memory for, and loading the player explosion sprites
	for (int a = 0; a < 8; a++) {
		PlayerExplosionGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
		dmaCopy((u8*)SpriteSheetTiles + TILESIZE * 8 + TILESIZE * a, PlayerExplosionGFXMem[a], 16 * 16);
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
	PlayerQuickSetup(&Player);
	
	// Setting up the bullet array
	BulletInitBulletArray(BulletArray, MAXBULLETCOUNT);

	// Test enemy spawning
	LoadRandomDifficulty1Enemies(Difficulty1EnemyIndexes);

	_Bool temp = 0;
	
	while(1) {
		// Clear the text
		consoleClear();
		// Get key presses
		scanKeys();
		int keys = keysHeld();
		int pressed = keysDown();
		// Frame number
		FrameNumber++;
		FrameNumber %= 60;

		// Reset
		if (keys & KEY_SELECT) {
			if (temp) {
				PlayerQuickSetup(&Player);  // Resetup the player
				LoadRandomDifficulty1Enemies(Difficulty1EnemyIndexes); // Enemy setup
				BulletInitBulletArray(BulletArray, MAXBULLETCOUNT);  // Clearing all bullets
				temp = 0;  // To prevent running once every frame
			}
		}
		else temp = 1;
		
		// Player movement and bullet firing
		PlayerHandle(keys);
		
		// Handle enemies here
		EnemyHandleAll(PlayerCenter, ScreenBoarder, 4);
	
		// Bullet handling
		BulletHandleBulletArray(BulletArray, MAXBULLETCOUNT, PlayableArea);
		BulletSpawnMineOffspring();

		// Bullet collision with player and enemies
		BulletCollisionWithPlayerAndEnemies();
		BulletCountAlive();
		
		// Player collision with enemies
		PlayerCheckCollisionAgainstAllEnemies();

		// Drawing and animating the player
		PlayerAnimate(&Player, FrameNumber, PlayerGFXMem, PlayerExplosionGFXMem);

		// Draw enemies here
		EnemyDrawAll(FrameNumber);

		// Drawing the bullets
		BulletDrawAll();
		
		// Displaying the player position and other stuff
		iprintf("\nX = %d\nY = %d\n", (int)Player.x, (int)Player.y);
		iprintf("Player Center [%d, %d]\n", PlayerCenter[0], PlayerCenter[1]);
		iprintf("Alive Bullets = %d\n", NumberOfAliveBullets);
		iprintf("Player Dead = %d\n", Player.dead);
		for (int i = 0; i < 8; i++) {
			iprintf("%d", EnemyEntityArray[i].dead);
		}
		iprintf("\n");
		for (int i = 0; i < 8; i++) {
			iprintf("%d,", EnemyEntityArray[i].counter);
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

	Easier setup of enemies
		Make a function to do it for me...

	Easy loading of multiple enemies from an array
		Randomise it

	Randomisation of spawning enemies

	Rework how enemies are handled, as there will only ever be a max of 8 enemise on screen at once

	Functions to easily handle the player and enemies
*/