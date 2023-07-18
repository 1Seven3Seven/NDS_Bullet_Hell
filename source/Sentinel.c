#include "Sentinel.h"

#include <nds.h>

#include <math.h>

#include "Constants.h"
#include "Entity.h"

void SentinelSetup(Entity *self, int x, int y) {
    EntitySetup(
            self,
            x, y,
            16, 16,
            SENTINEL_HEALTH,
            SENTINEL_TYPE,
            SENTINEL_BULLET_DELAY
    );
}

int SentinelSetupInEntityArray(Entity entity_array[], int entity_array_len, int x, int y) {
    int index = EntityGetNextAvailableIndexInArray(entity_array, entity_array_len);

    if (index == -1)
        return -1;

    SentinelSetup(&entity_array[index], x, y);

    return index;
}

void SentinelMove(Entity *self, int move_direction, int player_center[2], int hitbox_array[][4], int hitbox_array_len) {
    // Only move if not reloading
    if (self->current_bullet_delay == 0) {
        // Get my center
        int my_center[2];
        EntityGetCenterArray(self, my_center);

        // Get the difference
        int difference = player_center[move_direction] - my_center[move_direction];

        // Normalise the difference to length 1
        if (difference > 0)
            difference = 1;
        else if (difference < 0)
            difference = -1;

        // Calculating movement
        int movement[2] = {0, 0};
        movement[move_direction] = difference;

        // Move the sentinel
        EntityMove(self, movement[0], movement[1], hitbox_array, hitbox_array_len);

        // FLip the sprite to look at the player
        difference = player_center[!move_direction] - my_center[!move_direction];
        if (difference > 0)
            if (move_direction)
                self->h_flip = 0;
            else
                self->v_flip = 0;
        else if (move_direction)
            self->h_flip = 1;
        else
            self->v_flip = 1;
    }
}

void SentinelAnimate(Entity *self, int priority, int frame_number, int id_offset, int move_direction,
                     u16 *sentinel_gfx_mem[2][SENTINEL_ANIMATION_FRAMES], u16 *enemy_explosion_gfx_mem[]) {
    if (!self->dead) {
        // Increment frame number if necessary
        if (frame_number % 6 == 0) {
            self->animation_frame_number++;
            self->animation_frame_number %= 4;
        }

        // Grab the graphics memory
        u16 *gfx;
        if (self->current_bullet_delay == 0) {
            // Loaded animation
            gfx = sentinel_gfx_mem[move_direction][self->animation_frame_number];
        } else {
            // Reloading animation
            gfx = sentinel_gfx_mem[move_direction][4 + 3 - self->current_bullet_delay / 15];
        }

        // Draw the sentinel
        oamSet(
                &oamMain,
                1 + id_offset,
                self->x, self->y,
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
    } else {
        // Reduce counter
        if (self->counter > 0)
            self->counter--;

        // Draw sentinel explosion
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

void SentinelFireBullet(Entity *self, int move_direction, int player_center[2], Bullet bullet_array[],
                        int bullet_array_len) {
    if (self->current_bullet_delay > 0)
        self->current_bullet_delay--;
    else {
        // Get my center
        int my_center[2];
        EntityGetCenterArray(self, my_center);

        // Get the difference
        int difference = player_center[move_direction] - my_center[move_direction];

        // If directly looking at the player
        if (difference == 0) {
            // Get the difference in the other axis - for firing the bullet in the right direction
            difference = player_center[!move_direction] - my_center[!move_direction];

            // Find the direction for the bullet to travel
            float angle;
            if (move_direction) { // If firing horizontally
                if (difference > 0)
                    angle = 0;
                else
                    angle = M_PI;
            } else { // If firing vertically
                if (difference > 0)
                    angle = 3 * M_PI / 2;
                else
                    angle = M_PI / 2;
            }

            // Create the bullet
            self->current_bullet_delay = self->bullet_delay;
            BulletSetupInBulletArray(
                    bullet_array, bullet_array_len,
                    my_center[0] - 4, my_center[1] - 4,
                    8, 8,
                    angle,
                    1,
                    240,
                    1,
                    SENTINEL_BULLET
            );
        }
    }
}