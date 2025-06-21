#include "Miner.h"

#include <nds.h>

#include <math.h>

#include "Constants.h"
#include "Entity.h"

void MinerSetup(Entity *self, const int x, const int y)
{
    EntitySetup(
        self,
        x, y,
        16, 16,
        MINER_HEALTH,
        EntityType_Miner,
        MINER_BULLET_DELAY
    );
}

int MinerSetupInEntityArray(Entity entity_array[], const int entity_array_len, const int x, const int y)
{
    const int index = EntityGetNextAvailableIndexInArray(entity_array, entity_array_len);

    if (index == -1) { return -1; }

    MinerSetup(&entity_array[index], x, y);

    return index;
}

void MinerMove(Entity *self, float vector[2], int hitbox_array[][4], const int hitbox_array_len)
{
    if (self->current_bullet_delay > 0)
    {
        // While reloading
        // Move along the x-axis
        int collision = EntityMoveX(self, vector[0], hitbox_array, hitbox_array_len);
        // Reverse movement if a collision
        if (collision) { vector[0] = -vector[0]; }

        // Move along the y-axis
        collision = EntityMoveY(self, vector[1], hitbox_array, hitbox_array_len);
        // Reverse movement if a collision
        if (collision) { vector[1] = -vector[1]; }
    }
}

void MinerAnimate(
    Entity *self,
    const int priority,
    const int frame_number,
    const int id_offset,
    u16 *miner_gfx_mem[],
    u16 *enemy_explosion_gfx_mem[])
{
    if (!self->dead)
    {
        // Increment frame number if necessary
        if (frame_number % 6 == 0)
        {
            self->animation_frame_number += 1;
            self->animation_frame_number %= 4;
        }
        oamSet(
            &oamMain,
            1 + id_offset,
            (int) self->x, (int) self->y,
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
    }
    else
    {
        // Reduce counter
        if (self->counter > 0)
            self->counter--;

        // Animate the explosion
        oamSet(
            &oamMain,
            1 + id_offset,
            (int) self->x, (int) self->y,
            priority,
            0,
            SpriteSize_16x16,
            SpriteColorFormat_256Color,
            enemy_explosion_gfx_mem[7 - self->counter / 2],
            -1,
            false,
            !self->counter,
            self->v_flip,
            self->h_flip,
            false
        );
    }
}

void MinerPlaceMine(Entity *self, int *miner_place_mine_delay, Bullet bullet_array[], const int bullet_array_len)
{
    if (self->current_bullet_delay > 0) { self->current_bullet_delay--; }
    else
    {
        // Hold the mine for some time before placing
        if (*miner_place_mine_delay > 0) { *miner_place_mine_delay -= 1; }
        else
        {
            // Get my centre
            int my_center[2];
            EntityGetCenterArray(self, my_center);

            // Reset delays
            *miner_place_mine_delay = MINER_PLACE_MINE_DELAY;
            self->current_bullet_delay = self->bullet_delay;

            // Create the mine
            BulletSetupInBulletArray(
                bullet_array, bullet_array_len,
                (float) (my_center[0] - 5),
                (float) (my_center[1] - 5),
                8, 8,
                0,
                0,
                240,
                1,
                BulletType_MinerMine
            );
        }
    }
}

void MinerSpawnMineExplosion(Bullet bullet_array[], const int bullet_array_len)
{
    for (int i = 0; i < bullet_array_len; i++)
    {
        // For each bullet
        if (bullet_array[i].to_die)
        {
            // If the bullet is about to die
            if (bullet_array[i].type == BulletType_MinerMine)
            {
                // If the bullet is a mine, then get the centre
                int mine_centre[2];
                BulletGetCenterArray(&bullet_array[i], mine_centre);

                // Spawn the offspring
                for (int j = 0; j < 8; j++)
                {
                    BulletSetupInBulletArray(
                        bullet_array, bullet_array_len,
                        (float) (mine_centre[0] - 1),
                        (float) (mine_centre[1] - 1),
                        3, 3,
                        M_PI / 4 * j,
                        1,
                        60,
                        1,
                        BulletType_MinerMineBullet
                    );
                }
            }
        }
    }
}
