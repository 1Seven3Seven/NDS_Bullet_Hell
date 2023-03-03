#ifndef GAMELIB_H
#define GAMELIB_H

//---------------------------------------------------------------------------------
// Entity
//---------------------------------------------------------------------------------
typedef struct{
	float x;
	float y;
	int w;
	int h;
	
	int health;
	_Bool dead;
	
	int type;
}Entity;

void SetupEntity(Entity* self, int x, int y, int w, int h, int health, int type);
void GetRectArray(Entity* self, int rect_array[4]);
void GetCenterArray(Entity* self, int center_array[2]);
void SetRight(Entity* self, int right);
void SetBottom(Entity* self, int bottom);
void TakeDamage(Entity* self, int damage);
void PlayerMove(Entity* self, int movement[2]);

//---------------------------------------------------------------------------------
// Bullets
//---------------------------------------------------------------------------------
typedef struct{
	float x;
	float y;
	int w;
	int h;
	
	float vector[2];
	int damage;
	
	int lifetime;
	
	int type;
	
	_Bool alive;
	_Bool to_delete;
}Bullet;

void BulletInit(Bullet* self);
void SetupBullet(Bullet* self, float x, float y, float angle, int velocity, int damage, int lifetime, int type);
void BulletGetRectArray(Bullet* self, int rect_array[4]);
void BulletMove(Bullet* self);
void BulletUpdate(Bullet* self);

//---------------------------------------------------------------------------------
// Collision Detection
//---------------------------------------------------------------------------------

_Bool RectangleCollision(int rect1[4], int rect2[4]);

#endif