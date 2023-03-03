#include "GameLib.h" // As the struct needs to be defined in the header file
#include <math.h>

#define PI 3.14159265359

//---------------------------------------------------------------------------------
// Entity
//---------------------------------------------------------------------------------

void EntitySetup(Entity* self, int x, int y, int w, int h, int health, int type, int bullet_delay) {
	self->x = x;
	self->y = y;
	self->w = w;
	self->h = h;
	
	self->health = health;
	self->dead = 0;
	
	self->type = type;

	self->bullet_delay = bullet_delay;
	self->current_bullet_delay = 0;

	self->h_flip = 0;
	self->v_flip = 0;

	self->counter = 0;
	self->animation_frame_number = 0;
}

void EntityGetRectArray(Entity* self, int rect_array[4]) {
	rect_array[0] = self->x;
	rect_array[1] = self->y;
	rect_array[2] = self->w;
	rect_array[3] = self->h;
}

void EntityGetCenterArray(Entity* self, int center_array[2]) {
	center_array[0] = (int)(self->x + self->w / 2);
	center_array[1] = (int)(self->y + self->h / 2);
}

void EntitySetRight(Entity* self, int right) {
	self->x = right - self->w;
}

void EntitySetBottom(Entity* self, int bottom) {
	self->y = bottom - self->h;
}

void EntityTakeDamage(Entity* self, int damage) {
	self->health -= damage;
	if (self->health <= 0) {
		self->dead = 1;
	}
}

_Bool EntityMove(Entity* self, float x, float y, int HitboxArray[][4], int HitboxLen) {	
	int hitbox[4];
	_Bool hit_a_hitbox = 0;

	if (x != 0) {
		self->x += x;  // Move the entity
		EntityGetRectArray(self, hitbox);
		for (int i = 0; i < HitboxLen; i++) {
			if (RectangleCollision(hitbox, HitboxArray[i])) {  // If a collision
				// Adjust the entity position accordingly
				if (x > 0)  EntitySetRight(self, HitboxArray[i][0]);
				else self->x = RectangleGetRight(HitboxArray[i]);
				hit_a_hitbox = 1;
			}
		}
	}

	if (y != 0) {
		self->y += y;  // Move the entity
		EntityGetRectArray(self, hitbox);
		for (int i = 0; i < HitboxLen; i++) {
			if (RectangleCollision(hitbox, HitboxArray[i])) {  // If a collision
				// Adjust the entity position accordingly
				if (y > 0) EntitySetBottom(self, HitboxArray[i][1]);
				else self->y = RectangleGetBottom(HitboxArray[i]);
				hit_a_hitbox = 1;
			}
		}
	}

	return hit_a_hitbox;
}


//---------------------------------------------------------------------------------
// Bullets
//---------------------------------------------------------------------------------

void BulletInit(Bullet* self) {
	self->alive = 0;
	self->to_die = 0;
}

void BulletInitBulletArray(Bullet bullet_array[], int bullet_array_len) {
	for (int i = 0; i < bullet_array_len; i++) {
		BulletInit(&bullet_array[i]);
	}
}

void BulletSetup(Bullet* self, float x, float y, int w, int h, float angle, float velocity, int lifespan, int damage, int type) {
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

	self->type = type;
}

void BulletSetupInBulletArray(Bullet bullet_array[], int bullet_array_len, float x, float y, int w, int h, float angle, float velocity, int lifespan, int damage, int type) {
	for (int i = 0; i < bullet_array_len; i++) {
		if (!bullet_array[i].alive) {
			BulletSetup(
				&bullet_array[i],
				x, y,
				w, h,
				angle,
				velocity,
				lifespan,
				damage,
				type
			);
			break;
		}
	}
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

void BulletUpdate(Bullet* self) {
	BulletMove(self);
	self->lifespan--;
	if (self->lifespan == 0) {
		self->to_die = 1;
	}
}

void BulletHandleBulletArray(Bullet bullet_array[], int bullet_array_length, int screen_rectangle[4]) {
	int temp_bullet_hitbox[4];
	for (int i = 0; i < bullet_array_length; i++) {
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
}


//---------------------------------------------------------------------------------
// Collision Detection with rectangles & related things
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

int RectangleGetRight(int rect[4]) {
	return(rect[0] + rect[2]);
}

int RectangleGetBottom(int rect[4]) {
	return(rect[1] + rect[3]);
}


//---------------------------------------------------------------------------------
// Math stuff
//---------------------------------------------------------------------------------

float GetAngleFromOriginTo(int x, int y) {
	float angle;

	if (x != 0) angle = atanf(fabs(y) / fabs(x));
	else {
		if (y < 0) angle = PI / 2;
		else angle = 3 * PI / 2;
	}

	if (x < 0 && y < 0) angle = PI - angle;
	else if (x < 0 && y >= 0) angle += PI;
	else if (x > 0 && y > 0) angle = 2 * PI - angle;

	return angle;
}

void GetVectorFromAngle(float angle, float vector_array[2]) {
	vector_array[0] = cosf(angle);
	vector_array[1] = -sinf(angle);
}