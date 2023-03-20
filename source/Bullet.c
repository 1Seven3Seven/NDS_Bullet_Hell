#include "Bullet.h"

#include <math.h>

#include "Constants.h"
#include "Other.h"
#include "Entity.h"
#include "Player.h"

void BulletInit(Bullet *self) {
    self->to_die = 0;
    self->alive = 0;

    self->type = UNDEFINED_BULLET;
}

void BulletInitBulletArray(Bullet bullet_array[], int bullet_array_len) {
    for (int i = 0; i < bullet_array_len; i++) {
        BulletInit(&bullet_array[i]);
    }
}

void BulletSetup(Bullet *self, float x, float y, int w, int h, float angle, float velocity, int lifespan, int damage,
                 int type) {
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

    self->type = type;

    self->alive = 1;
}

int BulletSetupInBulletArray(Bullet bullet_array[], int bullet_array_len, float x, float y, int w, int h, float angle,
                             float velocity, int lifespan, int damage, int type) {
    int index = -1;

    for (int i = 0; i < bullet_array_len; i++) {
        if (!bullet_array[i].alive) {
            BulletSetup(
                    &bullet_array[i],
                    x, y,
                    w, h,
                    angle, velocity,
                    lifespan,
                    damage,
                    type
            );

            index = i;

            break;
        }
    }

    return index;
}

int BulletSetupInBulletArrayReversed(Bullet bullet_array[], int bullet_array_len, float x, float y, int w, int h,
                                     float angle, float velocity, int lifespan, int damage, int type) {
    int index = -1;

    for (int i = bullet_array_len - 1; i >= 0; i--) {
        if (!bullet_array[i].alive) {
            BulletSetup(
                    &bullet_array[i],
                    x, y,
                    w, h,
                    angle, velocity,
                    lifespan,
                    damage,
                    type
            );

            index = i;

            break;
        }
    }

    return index;
}

void BulletGetHitBox(Bullet *self, int rect_array[4]) {
    rect_array[0] = (int) self->x;
    rect_array[1] = (int) self->y;
    rect_array[2] = self->w;
    rect_array[3] = self->h;
}

void BulletGetCenterArray(Bullet *self, int center_array[2]) {
    center_array[0] = (int) (self->x + (float) self->w / 2);
    center_array[1] = (int) (self->y + (float) self->h / 2);
}

int BulletGetNumberAliveBulletsInBulletArray(Bullet bullet_array[], int bullet_array_len) {
    int num = 0;

    for (int i = 0; i < bullet_array_len; i++) {
        if (bullet_array[i].alive)
            num++;
    }

    return num;
}

void BulletMove(Bullet *self) {
    self->x += self->vector[0];
    self->y += self->vector[1];
}

void BulletUpdate(Bullet *self) {
    BulletMove(self);

    self->lifespan--;

    if (self->lifespan == 0) {
        self->to_die = 1;
    }
}

void BulletHandleBulletArray(Bullet bullet_array[], int bullet_array_len, int boundary_rectangle[4]) {
    int temp_bullet_hitbox[4];

    for (int i = 0; i < bullet_array_len; i++) { // For each bullet
        if (bullet_array[i].to_die) { // If the bullet is to die, kill it
            bullet_array[i].to_die = 0;
            bullet_array[i].alive = 0;
        } else { // If the bullet isn't to die
            // Update it
            BulletUpdate(&bullet_array[i]);

            // Check for if it is within the boundary
            BulletGetHitBox(&bullet_array[i], temp_bullet_hitbox);

            if (!RectangleCollision(boundary_rectangle, temp_bullet_hitbox)) { // If not then kill it
                bullet_array[i].to_die = 0;
                bullet_array[i].alive = 0;
            }
        }
    }
}

int BulletArrayCollisionWithPlayerAndEnemies(Bullet bullet_array[], int bullet_array_len, Entity enemy_array[],
                                             int enemy_array_len, Entity *player) {
    int player_hitbox[4], enemy_hitbox[4], bullet_hitbox[4];

    PlayerGetHitBox(player, player_hitbox);

    int player_collision_type = PLAYER_BULLET;

    // For each bullet
    for (int bullet_index = 0; bullet_index < bullet_array_len; bullet_index++) {
        // If the bullet is alive
        if (bullet_array[bullet_index].alive) {
            // Get the bullets hitbox
            BulletGetHitBox(&bullet_array[bullet_index], bullet_hitbox);
            // If the bullet is a player bullet
            if (bullet_array[bullet_index].type == PLAYER_BULLET) {
                // For each enemy
                for (int enemy_index = 0; enemy_index < enemy_array_len; enemy_index++) {
                    // If the enemy isn't dead
                    if (!enemy_array[enemy_index].dead) {
                        // Get the enemy hitbox
                        EntityGetHitBox(&enemy_array[enemy_index], enemy_hitbox);
                        // Check for a collision
                        if (RectangleCollision(bullet_hitbox, enemy_hitbox)) {
                            // If there is a collision damage the enemy
                            EntityTakeDamage(&enemy_array[enemy_index], bullet_array[bullet_index].damage);
                            // Bullet now dies
                            bullet_array[bullet_index].to_die = 1;
                        }
                    }
                }
            } else if (bullet_array[bullet_index].type > 0) { // If the bullet is an enemy bullet
                // If the player isn't dead
                if (!player->dead) {
                    // If there is a collision
                    if (RectangleCollision(player_hitbox, bullet_hitbox)) {
                        // Damage the player
                        EntityTakeDamage(player, bullet_array[bullet_index].damage);
                        // Bullet now dies
                        bullet_array[bullet_index].to_die = 1;
                        // The type of the bullet that collided with the player
                        player_collision_type = bullet_array[bullet_index].type;
                    }
                }
            }
        }
    }

    return player_collision_type;
}

void BulletSpawnDeathBullets(Bullet bullet_array[], int bullet_array_len, Entity enemy_array[], int enemy_array_len,
                             Entity *player) {
    for (int i = 0; i < enemy_array_len; i++) {
        // If the enemy just died
        if (enemy_array[i].dead && enemy_array[i].counter == 11) {
            // Get the center of the now dead enemy and the player
            int enemy_center[2], player_center[2];
            EntityGetCenterArray(&enemy_array[i], enemy_center);
            EntityGetCenterArray(player, player_center);

            // Find the angle
            float angle = GetAngleFromOriginTo(
                    player_center[0] - enemy_center[0],
                    player_center[1] - enemy_center[1]
            );

            // Bullet time
            BulletSetupInBulletArray(
                    bullet_array, bullet_array_len,
                    enemy_center[0] - 1, enemy_center[1] - 1,
                    3, 3,
                    angle,
                    1,
                    320,
                    1,
                    DEATH_BULLET
            );
            BulletSetupInBulletArray(
                    bullet_array, bullet_array_len,
                    enemy_center[0] - 1, enemy_center[1] - 1,
                    3, 3,
                    angle + M_PI_4,
                    1,
                    320,
                    1,
                    DEATH_BULLET
            );
            BulletSetupInBulletArray(
                    bullet_array, bullet_array_len,
                    enemy_center[0] - 1, enemy_center[1] - 1,
                    3, 3,
                    angle - M_PI_4,
                    1,
                    320,
                    1,
                    DEATH_BULLET
            );
        }
    }
}

void BulletDraw(Bullet *self, int id_offset, u16 *bullet_gfx_mem[][FRAMES_PER_BULLET]) {
    oamSet(
            &oamMain,
            BULLET_ID_START + id_offset,
            self->x, self->y,
            0,
            0,
            SpriteSize_16x16,
            SpriteColorFormat_256Color,
            bullet_gfx_mem[IndexForBulletType(self->type)][3 - self->lifespan / 6 % 4],
            -1,
            false,
            !self->alive,
            false,
            false,
            false
    );
}

void BulletDrawArray(Bullet bullet_array[], int bullet_array_len, u16 *bullet_gfx_mem[][FRAMES_PER_BULLET],
                     u16 *wild_bullet_gfx_mem[][FRAMES_PER_BULLET]) {
    for (int i = 0; i < bullet_array_len; i++) {
        if (bullet_array[i].type >= 0)
            BulletDraw(&bullet_array[i], i, bullet_gfx_mem);
        else
            BulletDraw(&bullet_array[i], i, wild_bullet_gfx_mem);
    }
}