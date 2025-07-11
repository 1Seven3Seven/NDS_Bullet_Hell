#include "Entity.h"

#include "Constants.h"
#include "Other.h"

void EntityInit(Entity *self)
{
    self->dead = 1;
    self->counter = 0;
}

void EntityInitEntityArray(Entity entity_array[], const int entity_array_len)
{
    for (int i = 0; i < entity_array_len; i++)
    {
        EntityInit(&entity_array[i]);
    }
}

void EntitySetup(
    Entity *self,
    const int x,
    const int y,
    const int w,
    const int h,
    const int health,
    const EntityType type,
    const int bullet_delay)
{
    self->x = (float) x;
    self->y = (float) y;
    self->w = w;
    self->h = h;

    self->health = health;
    self->dead = 0;

    self->type = type;

    self->bullet_delay = bullet_delay;
    self->current_bullet_delay = 0;

    self->h_flip = 0;
    self->v_flip = 0;

    self->counter = 12;
    self->animation_frame_number = 0;
}

int EntityGetNextAvailableIndexInArray(Entity entity_array[], const int entity_array_len)
{
    for (int i = 0; i < entity_array_len; i++)
    {
        if (entity_array[i].dead)
        {
            return i;
        }
    }

    return -1;
}

int EntitySetupInEntityArray(
    Entity entity_array[],
    const int entity_array_len,
    const int x,
    const int y,
    const int w,
    const int h,
    const int health,
    const int type,
    const int bullet_delay)
{
    const int index = EntityGetNextAvailableIndexInArray(entity_array, entity_array_len);

    if (index == -1)
    {
        return -1;
    }

    EntitySetup(
        &entity_array[index],
        x, y,
        w, h,
        health,
        type,
        bullet_delay
    );

    return index;
}

void EntityGetHitBox(const Entity *self, int rect_array[4])
{
    rect_array[0] = (int) self->x;
    rect_array[1] = (int) self->y;
    rect_array[2] = self->w;
    rect_array[3] = self->h;
}

void EntityGetCenterArray(const Entity *self, int center_array[2])
{
    center_array[0] = (int) (self->x + (float) self->w / 2);
    center_array[1] = (int) (self->y + (float) self->h / 2);
}

float EntityGetLeft(const Entity *self)
{
    return self->x;
}

float EntityGetRight(const Entity *self)
{
    return self->x + (float) self->w;
}

float EntityGetTop(const Entity *self)
{
    return self->y;
}

float EntityGetBottom(const Entity *self)
{
    return self->y + (float) self->h;
}

void EntitySetLeft(Entity *self, const float left)
{
    self->x = left;
}

void EntitySetRight(Entity *self, const float right)
{
    self->x = right - (float) self->w;
}

void EntitySetTop(Entity *self, const float top)
{
    self->y = top;
}

void EntitySetBottom(Entity *self, const float bottom)
{
    self->y = bottom - (float) self->h;
}

void EntityTakeDamage(Entity *self, const int damage)
{
    self->health -= damage;

    if (self->health <= 0)
    {
        self->dead = 1;
    }
}

int EntityMoveX(Entity *self, const float x, int hitbox_array[][4], const int hitbox_array_len)
{
    int hit_a_hitbox = 0;

    if (x != 0)
    {
        // Move the entity
        self->x += x;
        // Get the hitbox
        int hitbox[4];
        EntityGetHitBox(self, hitbox);

        // Check for a collision
        for (int i = 0; i < hitbox_array_len; i++)
        {
            if (RectangleCollision(hitbox, hitbox_array[i])) // If there is a collision, solve it
            {
                // Because you hit a hitbox on the x-axis
                hit_a_hitbox = COLLISION_X;

                // Solving the collision
                if (x > 0) // Moving to the right
                {
                    EntitySetRight(self, (float) RectangleGetLeft(hitbox_array[i]));
                }
                else // Moving to the left
                {
                    EntitySetLeft(self, (float) RectangleGetRight(hitbox_array[i]));
                }
            }
        }
    }

    return hit_a_hitbox;
}

int EntityMoveY(Entity *self, const float y, int hitbox_array[][4], const int hitbox_array_len)
{
    int hit_a_hitbox = 0;

    if (y != 0)
    {
        // Move the entity
        self->y += y;
        // Get the hitbox
        int hitbox[4];
        EntityGetHitBox(self, hitbox);

        // Check for a collision
        for (int i = 0; i < hitbox_array_len; i++)
        {
            if (RectangleCollision(hitbox, hitbox_array[i])) // If there is a collision
            {
                // Because you hit a hitbox on the y-axis
                hit_a_hitbox = COLLISION_Y;

                // Solving the collision
                if (RectangleCollision(hitbox, hitbox_array[i])) // If there is a collision, solve it
                {
                    if (y > 0) // Moving to the down
                    {
                        EntitySetBottom(self, (float) RectangleGetTop(hitbox_array[i]));
                    }
                    else // Moving to the up
                    {
                        EntitySetTop(self, (float) RectangleGetBottom(hitbox_array[i]));
                    }
                }
            }
        }
    }

    return hit_a_hitbox;
}

int EntityMove(Entity *self, const float x, const float y, int hitbox_array[][4], const int hitbox_array_len)
{
    const int x_movement = EntityMoveX(self, x, hitbox_array, hitbox_array_len);
    const int y_movement = EntityMoveY(self, y, hitbox_array, hitbox_array_len);

    return x_movement || y_movement;
}
