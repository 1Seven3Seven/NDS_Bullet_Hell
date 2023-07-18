#include "Player.h"

#include <nds.h>
#include <nds/arm9/sprite.h>

#include "Constants.h"
#include "Other.h"
#include "Entity.h"

void PlayerSetup(Entity *self) {
    EntitySetup(
            self,
            PLAYER_START_X, PLAYER_START_Y,
            13, 13,
            PLAYER_HEALTH,
            PLAYER_TYPE,
            PLAYER_BULLET_DELAY
    );

    self->counter = 16;
}

void
PlayerAnimate(Entity *self, int priority, int frame_number, u16 *player_gfx_mem[], u16 *player_explosion_gfx_mem[]) {
    if (!self->dead) {
        // Increment frame number if necessary
        if (!(frame_number % 6)) {
            self->animation_frame_number++;
            self->animation_frame_number %= 4;
        }

        // Draw player
        oamSet(
                &oamMain,
                0,
                self->x, self->y,
                priority,
                0,
                SpriteSize_16x16,
                SpriteColorFormat_256Color,
                player_gfx_mem[4 * (self->current_bullet_delay != 0) + self->animation_frame_number],
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

        // Draw player explosion
        oamSet(
                &oamMain,
                0,
                self->x, self->y,
                priority,
                0,
                SpriteSize_16x16,
                SpriteColorFormat_256Color,
                player_explosion_gfx_mem[7 - (int) (self->counter / 2)],
                -1,
                false,
                !self->counter,
                self->v_flip,
                self->h_flip,
                false
        );
    }
}

void PlayerMovement(Entity *self, int keys, int hitbox_array[][4], int hitbox_array_len) {
    // If dead don't run function
    if (self->dead)
        return;

    int x = 0, y = 0;
    if (keys & KEY_RIGHT)
        x = 1;
    if (keys & KEY_LEFT)
        x = -1;
    if (keys & KEY_DOWN)
        y = 1;
    if (keys & KEY_UP)
        y = -1;

    EntityMove(self, x, y, hitbox_array, hitbox_array_len);
}

void PlayerFireBullet(Entity *self, int keys, Bullet bullet_array[], int bullet_array_len, int hitbox_array[][4],
                      int hitbox_array_len) {
    // If dead don't run function
    if (self->dead)
        return;

    // Reduce bullet delay
    if (self->current_bullet_delay > 0)
        self->current_bullet_delay--;
    else {
        // If fire keys pressed
        if (keys & KEY_A || keys & KEY_B || keys & KEY_X || keys & KEY_Y) {
            // Finding bullet direction
            int x = 0;
            int y = 0;
            if (keys & KEY_A)
                x += 1;
            if (keys & KEY_Y)
                x -= 1;
            if (keys & KEY_B)
                y += 1;
            if (keys & KEY_X)
                y -= 1;

            // Find the angle
            float angle = GetAngleFromOriginTo(x, y);

            // Reset the bullet delay
            self->current_bullet_delay = self->bullet_delay;

            // Get the player center
            int player_center[2];
            EntityGetCenterArray(self, player_center);

            // Create the bullet
            BulletSetupInBulletArray(
                    bullet_array, bullet_array_len,
                    player_center[0] - 2, player_center[1] - 2,
                    5, 5,
                    angle,
                    2,
                    120,
                    1,
                    PLAYER_BULLET
            );

            // Move player in opposite direction
            EntityMove(self, -x, -y, hitbox_array, hitbox_array_len);
        }
    }
}

void PlayerGetHitBox(Entity *self, int rect_array[4]) {
    EntityGetHitBox(self, rect_array);
    rect_array[0] += 2;
    rect_array[1] += 2;
    rect_array[2] -= 4;
    rect_array[3] -= 4;
}