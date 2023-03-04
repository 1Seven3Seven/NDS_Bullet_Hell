#include "Miner.h"

#include <nds.h>

#include <math.h>

#include "Constants.h"
#include "Entity.h"

void MinerSetup(Entity *self, int x, int y) {
    EntitySetup(
            self,
            x, y,
            16, 16,
            MINER_HEALTH,
            MINER_TYPE,
            MINER_BULLET_DELAY
    );
}

int MinerSetupInEntityArray(Entity entity_array[], int entity_array_len, int x, int y) {
    int index = EntityGetNextAvailableIndexInArray(entity_array, entity_array_len);

    if (index == -1)
        return -1;

    MinerSetup(&entity_array[index], x, y);

    return index;
}

void MinerMove(Entity *self, float vector[2], int hitbox_array[][4], int hitbox_array_len) {
    if (self->current_bullet_delay > 0) { // While reloading
        // Move along x-axis
        int collision = EntityMoveX(self, vector[0], hitbox_array, hitbox_array_len);
        // Reverse movement if a collision
        if (collision)
            vector[0] = -vector[0];

        // Move along y-axis
        collision = EntityMoveY(self, vector[1], hitbox_array, hitbox_array_len);
        // Reverse movement if a collision
        if (collision)
            vector[1] = -vector[1];
    }
}

void MinerAnimate(Entity *self, int priority, int frame_number, int id_offset, u16 *miner_gfx_mem[],
                  u16 *enemy_explosion_gfx_mem[]) {
    if (!self->dead) {
        // Increment frame number if necessary
        if (frame_number % 6 == 0) {
            self->animation_frame_number += 1;
            self->animation_frame_number %= 4;
        }
        oamSet(
                &oamMain,
                1 + id_offset,
                self->x, self->y,
                priority,
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
    } else {
        // Reduce counter
        if (self->counter > 0)
            self->counter--;

        // Animate the explosion
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
                false
        );
    }
}

void MinerPlaceMine(Entity *self, int *miner_place_mine_delay, Bullet bullet_array[], int bullet_array_len) {
    if (self->current_bullet_delay > 0)
        self->current_bullet_delay--;
    else {
        // Hold the mine for some time before placing
        if (*miner_place_mine_delay > 0)
            *miner_place_mine_delay -= 1;
        else {
            // Get my center
            int my_center[2];
            EntityGetCenterArray(self, my_center);

            // Reset delays
            *miner_place_mine_delay = MINER_PLACE_MINE_DELAY;
            self->current_bullet_delay = self->bullet_delay;

            // Create the mine
            BulletSetupInBulletArray(
                    bullet_array, bullet_array_len,
                    my_center[0] - 5, my_center[1] - 5,
                    8, 8,
                    0,
                    0,
                    240,
                    1,
                    MINER_MINE
            );
        }
    }
}

void MinerSpawnMineExplosion(Bullet bullet_array[], int bullet_array_len) {
    for (int i = 0; i < bullet_array_len; i++) { // For each bullet
        if (bullet_array[i].to_die) { // If the bullet is about to die
            if (bullet_array[i].type == MINER_MINE) { // If the bullet is a mine
                // Get the center
                int my_center[2];
                BulletGetCenterArray(&bullet_array[i], my_center);

                // Spawn the offspring
                for (int j = 0; j < 8; j++) {
                    BulletSetupInBulletArray(
                            bullet_array, bullet_array_len,
                            my_center[0] - 1, my_center[1] - 1,
                            3, 3,
                            M_PI / 4 * j,
                            1,
                            60,
                            1,
                            MINER_MINE_BULLET
                    );
                }
            }
        }
    }
}