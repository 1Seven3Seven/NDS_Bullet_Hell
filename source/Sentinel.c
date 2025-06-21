#include "Sentinel.h"

#include <nds.h>

#include <math.h>

#include "Constants.h"
#include "Entity.h"

void SentinelSetup(Entity *self, const int x, const int y)
{
    EntitySetup(
        self,
        x, y,
        16, 16,
        SENTINEL_HEALTH,
        EntityType_Sentinel,
        SENTINEL_BULLET_DELAY
    );
}

int SentinelSetupInEntityArray(Entity entity_array[], const int entity_array_len, const int x, const int y)
{
    const int index = EntityGetNextAvailableIndexInArray(entity_array, entity_array_len);

    if (index == -1)
    {
        return -1;
    }

    SentinelSetup(&entity_array[index], x, y);

    return index;
}

void SentinelMove(
    Entity *self,
    const int move_direction,
    const int player_center[2],
    int hitbox_array[][4],
    const int hitbox_array_len)
{
    // Only move if not reloading
    if (self->current_bullet_delay == 0)
    {
        // Get my centre
        int my_center[2];
        EntityGetCenterArray(self, my_center);

        // Get the difference
        int difference = player_center[move_direction] - my_center[move_direction];

        // Normalise the difference to length 1
        if (difference > 0) { difference = 1; }
        else if (difference < 0) { difference = -1; }

        // Calculating movement
        int movement[2] = {0, 0};
        movement[move_direction] = difference;

        // Move the sentinel
        EntityMove(self, (float) movement[0], (float) movement[1], hitbox_array, hitbox_array_len);

        // FLip the sprite to look at the player
        difference = player_center[!move_direction] - my_center[!move_direction];
        if (difference > 0)
        {
            if (move_direction) { self->h_flip = 0; }
            else { self->v_flip = 0; }
        }
        else if (move_direction) { self->h_flip = 1; }
        else { self->v_flip = 1; }
    }
}

void SentinelAnimate(
    Entity *self,
    const int priority,
    const int frame_number,
    const int id_offset,
    const int move_direction,
    u16 *sentinel_gfx_mem[2][SENTINEL_ANIMATION_FRAMES],
    u16 *enemy_explosion_gfx_mem[])
{
    if (!self->dead)
    {
        // Increment frame number if necessary
        if (frame_number % 6 == 0)
        {
            self->animation_frame_number++;
            self->animation_frame_number %= 4;
        }

        // Grab the graphics memory
        u16 *gfx;
        if (self->current_bullet_delay == 0)
        {
            // Loaded animation
            gfx = sentinel_gfx_mem[move_direction][self->animation_frame_number];
        }
        else
        {
            // Reloading animation
            gfx = sentinel_gfx_mem[move_direction][4 + 3 - self->current_bullet_delay / 15];
        }

        // Draw the sentinel
        oamSet(
            &oamMain,
            1 + id_offset,
            (int) self->x,
            (int) self->y,
            priority,
            0,
            SpriteSize_16x16,
            SpriteColorFormat_256Color,
            gfx,
            -1,
            false,
            self->dead,
            self->h_flip,
            self->v_flip,
            false
        );
    }
    else
    {
        // Reduce counter
        if (self->counter > 0) { self->counter--; }

        // Draw sentinel explosion
        oamSet(
            &oamMain,
            1 + id_offset,
            (int) self->x,
            (int) self->y,
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

void SentinelFireBullet(
    Entity *self,
    const int move_direction,
    const int player_center[2],
    Bullet bullet_array[],
    const int bullet_array_len)
{
    if (self->current_bullet_delay > 0) { self->current_bullet_delay--; }
    else
    {
        // Get my centre
        int my_center[2];
        EntityGetCenterArray(self, my_center);

        // Get the difference
        int difference = player_center[move_direction] - my_center[move_direction];

        // If directly looking at the player
        if (difference == 0)
        {
            // Get the difference in the other axis - for firing the bullet in the right direction
            difference = player_center[!move_direction] - my_center[!move_direction];

            // Find the direction for the bullet to travel
            float angle;
            if (move_direction)
            {
                // If firing horizontally
                if (difference > 0) { angle = 0; }
                else { angle = M_PI; }
            }
            else
            {
                // If firing vertically
                if (difference > 0) { angle = 3 * (float) M_PI / 2; }
                else { angle = M_PI / 2; }
            }

            // Create the bullet
            self->current_bullet_delay = self->bullet_delay;
            BulletSetupInBulletArray(
                bullet_array, bullet_array_len,
                (float) (my_center[0] - 4),
                (float) (my_center[1] - 4),
                8, 8,
                angle,
                1,
                240,
                1,
                BulletType_SentinelBullet
            );
        }
    }
}
