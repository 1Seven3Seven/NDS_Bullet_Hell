#include "Shredder.h"

#include <nds.h>

#include "Constants.h"
#include "Other.h"
#include "Entity.h"

void ShredderSetup(Entity *self, const int x, const int y)
{
    EntitySetup(
        self,
        x, y,
        15, 15,
        SHREDDER_HEALTH,
        EntityType_Shredder,
        SHREDDER_BULLET_DELAY
    );
}

int ShredderSetupInEntityArray(Entity entity_array[], const int entity_array_len, const int x, const int y)
{
    const int index = EntityGetNextAvailableIndexInArray(entity_array, entity_array_len);

    if (index == -1) { return -1; }

    ShredderSetup(&entity_array[index], x, y);

    return index;
}

void ShredderMove(
    Entity *self,
    float vector[2],
    const int player_center[2],
    int hitbox_array[][4],
    const int hitbox_array_len)
{
    if (self->current_bullet_delay == 0)
    {
        // If not waiting, then move by the vector
        const int collision = EntityMove(self, vector[0], vector[1], hitbox_array, hitbox_array_len);

        // If there is a collision, wait for bullet delay
        if (collision)
        {
            self->current_bullet_delay = self->bullet_delay;
        }
    }
    else
    {
        // If waiting, then reduce the wait
        self->current_bullet_delay--;

        if (self->current_bullet_delay == 0)
        {
            // If no longer waiting, then get my centre
            int my_center[2];
            EntityGetCenterArray(self, my_center);

            // Create a new vector
            GetVectorFromTo(
                (float) my_center[0], (float) my_center[1],
                (float) player_center[0], (float) player_center[1],
                SHREDDER_SPEED,
                vector
            );
        }
    }
}

void ShredderAnimate(
    Entity *self,
    const int priority,
    const int frame_number,
    const int id_offset, u16 *shredder_gfx_mem[],
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
        // Draw the shredder
        oamSet(
            &oamMain,
            1 + id_offset,
            (int) self->x, (int) self->y,
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
    }
    else
    {
        // Reduce counter
        if (self->counter > 0)
            self->counter--;

        // EXPLOSION!!!
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
            false);
    }
}
