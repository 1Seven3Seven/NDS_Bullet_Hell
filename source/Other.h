#ifndef BULLET_HELL_OTHER_H
#define BULLET_HELL_OTHER_H

// Check for a collision between the two given rectangles
int RectangleCollision(int rect1[4], int rect2[4]);

// Get the x value of the left of the rectangle
int RectangleGetLeft(int rect[4]);

// Get the x value of the right of the rectangle
int RectangleGetRight(int rect[4]);

// Get the y value of the top of the rectangle
int RectangleGetTop(int rect[4]);

// Get the y value of the bottom of the rectangle
int RectangleGetBottom(int rect[4]);

// Returns the angle from the positive horizontal to a line drawn to this point
float GetAngleFromOriginTo(float x, float y);

// Creates a unit vector from the given angle and places it in the vector array
void GetUnitVectorFromAngle(float angle, float vector_array[2]);

// Finds the vector from (x1, y1) to (x2, y2), makes it be of length magnitude and places it in the vector array
void GetVectorFromTo(float x1, float y1, float x2, float y2, float magnitude, float vector_array[2]);

// Shuffles the given array of ints
void ShuffleIntArray(int array[], int array_len);

// Hides every sprite
void HideEverySprite();

#endif // BULLET_HELL_OTHER_H