#include "GameLib.h" // As the struct needs to be defined in the header file
#include <math.h>

//---------------------------------------------------------------------------------
// Entity
//---------------------------------------------------------------------------------

void EntitySetup(Entity* self, int x, int y, int w, int h, int health, int type) {
	self->x = x;
	self->y = y;
	self->w = w;
	self->h = h;
	
	self->health = health;
	self->dead = 0;
	
	self->type = type;
}

void EntityGetRectArray(Entity* self, int rect_array[4]) {
	rect_array[0] = (int)self->x;
	rect_array[1] = (int)self->y;
	rect_array[2] = self->w;
	rect_array[3] = self->h;
}

void EntityGetCenterArray(Entity* self, int center_array[2]) {
	center_array[0] = (int)self->x + self->w / 2;
	center_array[1] = (int)self->y + self->h / 2;
}

void EntitySetRight(Entity* self, int right) {
	self->x = right - self->w;
}

void EntitySetBottom(Entity* self, int bottom) {
	self->y = bottom - self->h;
}

void EntityTakeDamage(Entity* self, int damage) {
	self->health -= damage;
	if (self->health < 0) {
		self->dead = 1;
	}
}

void EntityMove(Entity* self, int movement[2]) {
	self->x += movement[0];
	self->y += movement[1];
}

//---------------------------------------------------------------------------------
// Bullets
//---------------------------------------------------------------------------------

void BulletInit(Bullet* self) {
	self->alive = 0;
	self->to_die = 0;
}

void BulletSetup(Bullet* self, float x, float y, int w, int h, float angle, int velocity, int lifespan, int damage) {
	self->x = x;
	self->y = y;
	self->w = w;
	self->h = h;
	
	self->angle = angle;
	self->velocity = velocity;
	self->vector[0] = cosf(angle) * velocity;
	self->vector[1] = -sinf(angle) * velocity;
	
	self->lifespan = lifespan;
	self->damage = damage;
	
	self->alive = 1;
}

void BulletGetRectArray(Bullet* self, int rect_array[4]) {
	rect_array[0] = (int)self->x;
	rect_array[1] = (int)self->y;
	rect_array[2] = self->w;
	rect_array[3] = self->h;
}

void BulletGetCenterArray(Bullet* self, int center_array[2]) {
	center_array[0] = (int)self->x + self->w / 2;
	center_array[1] = (int)self->y + self->h / 2;
}

void BulletMove(Bullet* self) {
	self->x += self->vector[0];
	self->y += self->vector[1];
}

_Bool BulletSetupInArray(Bullet BulletArray[], int ArrayLength, float x, float y, int w, int h, float angle, int velocity, int lifespan, int damage) {
	for (int i = 0; i < ArrayLength; i++) {
		if (BulletArray[i].alive == 0) {
			BulletSetup(&BulletArray[i], x, y, w, h, angle, velocity, lifespan, damage);
			return 1;
		}
	}
	return 0;
}

void BulletUpdate(Bullet* self) {
	BulletMove(self);
	self->lifespan--;
	if (self->lifespan == 0) {
		self->to_die = 1;
	}
}

//---------------------------------------------------------------------------------
// Collision Detection & related things
//---------------------------------------------------------------------------------

_Bool RectangleCollision(int rect1[4], int rect2[4]) {

	if (rect1[0] <  rect2[0] + rect2[2]) { // If rect1_left < rect2_right
		if (rect1[0] + rect1[2] > rect2[0]) { // If rect1_right > rect2_left
			if (rect1[1] + rect1[3] > rect2[1]) { // If rect1_bottom > rect2_top
				if (rect1[1] < rect2[1] + rect2[3]) { // If rect1_top < rect2_bottom
					return(1);
				}
			}
		}
	}

	return(0);
}

int GetRightOfRectangle(int rect[4]) {
	return(rect[0] + rect[2]);
}
int GetBottomOfRectangle(int rect[4]) {
	return(rect[1] + rect[3]);
}