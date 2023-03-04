#include "Shredder.h"

#include <nds.h>

#include "Constants.h"
#include "Other.h"
#include "Entity.h"

void ShredderSetup(Entity *self, int x, int y) {
    EntitySetup(
            self,
            x, y,
            15, 15,
            SHREDDER_HEALTH,
            SHREDDER_TYPE,
            SHREDDER_BULLET_DELAY
    );
}

int ShredderSetupInEntityArray(Entity entity_array[], int entity_array_len, int x, int y) {
    int index = EntityGetNextAvailableIndexInArray(entity_array, entity_array_len);

    if (index == -1)
        return -1;

    ShredderSetup(&entity_array[index], x, y);

    return index;
}

void ShredderMove(Entity *self, float vector[2], int player_center[2], int hitbox_array[][4], int hitbox_array_len) {
    if (self->current_bullet_delay == 0) { // If not waiting
        // Move by the vector
        int collision = EntityMove(self, vector[0], vector[1], hitbox_array, hitbox_array_len);

        // If there is a collision, wait for bullet delay
        if (collision)
            self->current_bullet_delay = self->bullet_delay;

    } else { // If waiting
        // Reduce wait
        self->current_bullet_delay--;

        if (self->current_bullet_delay == 0) { // If no longer waiting
            // Get my center
            int my_center[2];
            EntityGetCenterArray(self, my_center);

            // Create new vector
            GetVectorFromTo(
                    my_center[0], my_center[1],
                    player_center[0], player_center[1],
                    SHREDDER_SPEED,
                    vector
            );
        }
    }
}

void ShredderAnimate(Entity *self, int priority, int frame_number, int id_offset, u16 *shredder_gfx_mem[],
                     u16 *enemy_explosion_gfx_mem[]) {
    if (!self->dead) {
        // Increment frame number if necessary
        if (frame_number % 6 == 0) {
            self->animation_frame_number += 1;
            self->animation_frame_number %= 4;
        }
        // Draw the shredder
        oamSet(
                &oamMain,
                1 + id_offset,
                self->x, self->y,
                priority,
                0,
                SpriteSize_16x16,
                SpriteColorFormat_256Color,
                shredder_gfx_mem[self->animation_frame_number],
                -1,
                false,
                false,
                self->v_flip,
                self->h_flip,
                false);
    } else {
        // Reduce counter
        if (self->counter > 0)
            self->counter--;

        // EXPLOSION!!!
        oamSet(
                &oamMain,
                1 + id_offset,
                self->x, self->y,
                priority,
                0,
                SpriteSize_16x16,
                SpriteColorFormat_256Color,
                enemy_explosion_gfx_mem[7 - (int) (self->counter / 2)],
                -1,
                false,
                !self->counter,
                self->v_flip,
                self->h_flip,
                false);
    }
}