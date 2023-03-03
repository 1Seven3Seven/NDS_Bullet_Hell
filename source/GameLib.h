#ifndef GAMELIB_H
#define GAMELIB_H

//---------------------------------------------------------------------------------
// Entity
//---------------------------------------------------------------------------------

typedef struct {
	int x;
	int y;
	int w;
	int h;
	
	int health;
	_Bool dead;
	
	int type;

	int last_movement[2];
} Entity;

void EntitySetup(Entity* self, int x, int y, int w, int h, int health, int type);
void EntityGetRectArray(Entity* self, int rect_array[4]);
void EntityGetCenterArray(Entity* self, int center_array[2]);
void EntitySetRight(Entity* self, int right);
void EntitySetBottom(Entity* self, int bottom);
void EntityTakeDamage(Entity* self, int damage);
void EntityMove(Entity* self, int movement[2]);
void EntityMoveAmount(Entity* self, int x, int y, int HitboxArray[][4], int HitboxLen);

//---------------------------------------------------------------------------------
// Bullets
//---------------------------------------------------------------------------------

typedef struct {
	float x;
	float y;
	int w;
	int h;
	
	float angle;
	float velocity;
	float vector[2];
	
	int lifespan;
	int damage;
	
	_Bool alive;  // Whether or not the bullet should be handled
	_Bool to_die;  // Whether or not the bullet should stop being handled

	int type;  // A indication of what type of bullet this is
	
} Bullet;

void BulletInit(Bullet* self);
void BulletInitBulletArray(Bullet bullet_array[], int bullet_array_len);
void BulletSetup(Bullet* self, float x, float y, int w, int h, float angle, int velocity, int lifespan, int damage, int type);
void BulletSetupInBulletArray(Bullet bullet_array[], int bullet_array_len, float x, float y, int w, int h, float angle, int velocity, int lifespan, int damage, int type);
void BulletGetRectArray(Bullet* self, int rect_array[4]);
void BulletGetCenterArray(Bullet* self, int center_array[2]);
void BulletMove(Bullet* self);
void BulletUpdate(Bullet* self);
void BulletHandleBulletArray(Bullet bullet_array[], int bullet_array_length, int screen_rectangle[4]);

//---------------------------------------------------------------------------------
// Collision Detection
//---------------------------------------------------------------------------------

_Bool RectangleCollision(int rect1[4], int rect2[4]);
int RectangleGetRight(int rect[4]);
int RectangleGetBottom(int rect[4]);

//---------------------------------------------------------------------------------
// Math stuff
//---------------------------------------------------------------------------------

float GetAngleFromOriginTo(int x, int y);

#endif