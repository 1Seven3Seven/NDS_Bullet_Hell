#ifndef GAMELIB_H
#define GAMELIB_H

//---------------------------------------------------------------------------------
// Entity
//---------------------------------------------------------------------------------
typedef struct {
	float x;
	float y;
	int w;
	int h;
	
	int health;
	_Bool dead;
	
	int type;
} Entity;

void EntitySetup(Entity* self, int x, int y, int w, int h, int health, int type);
void EntityGetRectArray(Entity* self, int rect_array[4]);
void EntityGetCenterArray(Entity* self, int center_array[2]);
void EntitySetRight(Entity* self, int right);
void EntitySetBottom(Entity* self, int bottom);
void EntityTakeDamage(Entity* self, int damage);
void EntityMove(Entity* self, int movement[2]);

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
	
} Bullet;

void BulletInit(Bullet* self);
void BulletSetup(Bullet* self, float x, float y, int w, int h, float angle, int velocity, int lifespan, int damage);
void BulletGetRectArray(Bullet* self, int rect_array[4]);
void BulletGetCenterArray(Bullet* self, int center_array[2]);
void BulletMove(Bullet* self);
_Bool BulletSetupInArray(Bullet BulletArray[], int ArrayLength, float x, float y, int w, int h, float angle, int velocity, int lifespan, int damage);
void BulletUpdate(Bullet* self);

//---------------------------------------------------------------------------------
// Collision Detection
//---------------------------------------------------------------------------------

_Bool RectangleCollision(int rect1[4], int rect2[4]);
int GetRightOfRectangle(int rect[4]);
int GetBottomOfRectangle(int rect[4]);

#endif