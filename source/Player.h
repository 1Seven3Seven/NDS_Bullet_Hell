#ifndef BULLET_HELL_PLAYER_H
#define BULLET_HELL_PLAYER_H

#include <nds.h>

#include "Bullet.h"
#include "Entity.h"

// Sets up the player to start in the center of the screen
void PlayerSetup(Entity *self);

// Animates the player
void PlayerAnimate(Entity *self, int priority, int frame_number, u16 *player_gfx_mem[], u16 *player_explosion_gfx_mem[]);

// Moves the player with respect to the keys pressed
void PlayerMovement(Entity *self, int keys, int hitbox_array[][4], int hitbox_array_len);

// Attempts to fire a bullet from the player depending on the keys pressed
void PlayerFireBullet(Entity *self, int keys, Bullet bullet_array[], int bullet_array_len, int hitbox_array[][4],
                      int hitbox_array_len);

// Gets the player hitbox, reduced in size to make it easier for the player
void PlayerGetHitBox(Entity *self, int rect_array[4]);

#endif // BULLET_HELL_PLAYER_H