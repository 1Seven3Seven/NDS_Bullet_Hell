#ifndef BULLET_HELL_SHREDDER_H
#define BULLET_HELL_SHREDDER_H

#include <nds.h>

#include "Entity.h"

// Sets a shredder at the given position
void ShredderSetup(Entity *self, int x, int y);

// Sets up a shredder in the entity array
// Same returns as EntitySetupInEntityArray
int ShredderSetupInEntityArray(Entity entity_array[], int entity_array_len, int x, int y);

// Moves the shredder with respect to its movement vector
// Changes the values within the movement vector when necessary
void ShredderMove(Entity *self, float vector[2], int player_center[2], int hitbox_array[][4], int hitbox_array_len);

// Animates the shredder
void ShredderAnimate(Entity *self, int priority, int frame_number, int id_offset, u16 *shredder_gfx_mem[],
                     u16 *enemy_explosion_gfx_mem[]);

#endif //BULLET_HELL_SHREDDER_H
