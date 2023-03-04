#ifndef BULLET_HELL_ENTITY_H
#define BULLET_HELL_ENTITY_H

// Contains information regarding an entity
typedef struct {
    // Position
    float x, y;

    // Size
    int w, h;

    // Health
    int health;
    // If the entity is dead
    int dead;

    // What type of entity this is, also relates to the bullets fired by the entity
    int type;

    // For firing bullets
    int bullet_delay;
    int current_bullet_delay;

    // For flipping sprites
    int h_flip;
    int v_flip;

    // Counter
    int counter;
    // What frame of the animation is currently being displayed
    int animation_frame_number;
} Entity;

/*
 *  Setup
 */

// Changes the necessary values to make the entity not be updated
// Used for entities you have created but don't want to use immediately
void EntityInit(Entity *self);

// Calls EntityInit on every entity struct in the given array
void EntityInitEntityArray(Entity entity_array[], int entity_array_len);

// Returns the index of the next available entity in the given entity array
// If none is available then returns -1
int EntityGetNextAvailableIndexInArray(Entity entity_array[], int entity_array_len);

// Sets up the given entity
void EntitySetup(Entity *self, int x, int y, int w, int h, int health, int type, int bullet_delay);

// Attempts to set up an entity in the given array of entities
// Will return the index of the setup entity if the setup was successful
// Will return -1 if not successful, there was no free entities
int EntitySetupInEntityArray(Entity entity_array[], int entity_array_len, int x, int y, int w, int h, int health,
                             int type, int bullet_delay);

/*
 *  Retrieving information
 */

// Get the rectangle of the hitbox for the entity and place it in the given array
void EntityGetHitBox(Entity *self, int rect_array[4]);

// Get the center of the entity and place it in the given array
void EntityGetCenterArray(Entity *self, int center_array[2]);

// Get the x-coordinate of the left of the entity
float EntityGetLeft(Entity *self);

// Get the x-coordinate of the right of the entity
float EntityGetRight(Entity *self);

// Get the y-coordinate of the top of the entity
float EntityGetTop(Entity *self);

// Get the y-coordinate of the bottom of the entity
float EntityGetBottom(Entity *self);

/*
 *  Setting position
 */

// Set the x-coordinate of the left of the entity
void EntitySetLeft(Entity *self, float left);

// Set the x-coordinate of the right of the entity
void EntitySetRight(Entity *self, float right);

// Set the y-coordinate of the top of the entity
void EntitySetTop(Entity *self, float top);

// Set the y-coordinate of the bottom of the entity
void EntitySetBottom(Entity *self, float bottom);

/*
 *  Moving
 */

// Moves the entity by the given movement along the x-axis
// Returns COLLISION_X if a hitbox was hit, 0 if otherwise
int EntityMoveX(Entity *self, float x, int hitbox_array[][4], int hitbox_array_len);

// Moves the entity by the given movement along the y-axis
// Returns COLLISION_Y if a hitbox was hit, 0 if otherwise
int EntityMoveY(Entity *self, float y, int hitbox_array[][4], int hitbox_array_len);

// Moves the entity by the given movement
// Returns COLLISION_Y bitwise or COLLISION_X if a hitbox was hit, 0 if otherwise
int EntityMove(Entity *self, float x, float y, int hitbox_array[][4], int hitbox_array_len);

/*
 * Other
 */

void EntityTakeDamage(Entity *self, int damage);

#endif // BULLET_HELL_ENTITY_H