#include "Other.h"

#include <nds.h>

#include <math.h>
#include <stdlib.h>

#include "GFX.h"

int RectangleCollision(int rect1[4], int rect2[4]) {
    if (RectangleGetLeft(rect1) < RectangleGetRight(rect2))             // If rect1_left < rect2_right
        if (RectangleGetRight(rect1) > RectangleGetLeft(rect2))         // If rect1_right > rect2_left
            if (RectangleGetBottom(rect1) > RectangleGetTop(rect2))     // If rect1_bottom > rect2_top
                if (RectangleGetTop(rect1) < RectangleGetBottom(rect2)) // If rect1_top < rect2_bottom
                    return 1;

    return 0;
}

int RectangleGetLeft(int rect[4]) {
    return rect[0];
}

int RectangleGetRight(int rect[4]) {
    return rect[0] + rect[2];
}

int RectangleGetTop(int rect[4]) {
    return rect[1];
}

int RectangleGetBottom(int rect[4]) {
    return rect[1] + rect[3];
}

float GetAngleFromOriginTo(float x, float y) {
    float angle;

    if (x != 0) angle = atanf(fabsf(y) / fabsf(x));
    else {
        if (y < 0) angle = M_PI / 2;
        else angle = 3 * M_PI / 2;
    }

    if (x < 0 && y < 0) angle = M_PI - angle;
    else if (x < 0 && y >= 0) angle += M_PI;
    else if (x > 0 && y > 0) angle = 2 * M_PI - angle;

    return angle;
}

void GetUnitVectorFromAngle(float angle, float vector_array[2])
{
    vector_array[0] = cosf(angle);
    vector_array[1] = -sinf(angle);
}

void GetVectorFromTo(float x1, float y1, float x2, float y2, float magnitude, float vector_array[2]) {
    // Vector time
    vector_array[0] = x2 - x1;
    vector_array[1] = y2 - y1;

    // Magnitude
    float old_magnitude = sqrt(vector_array[0] * vector_array[0] + vector_array[1] * vector_array[1]);

    // Make length be magnitude
    vector_array[0] *= magnitude / old_magnitude;
    vector_array[1] *= magnitude / old_magnitude;
}

void ShuffleIntArray(int array[], int array_len) {
    if (array_len > 1) {
        for (int i = array_len - 1; i > 0; i--) {
            int j = rand() % (i + 1);

            int tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
        }
    }
}

void HideEverySprite() {
    for (int i = 0; i < 128; i++) {
        oamSet(
                &oamMain,
                i,
                0, 0,
                0,
                0,
                SpriteSize_16x16,
                SpriteColorFormat_256Color,
                GFXAllSpriteGFX.PortalGFXMem[0],
                -1,
                false,
                true,
                false,
                false,
                false
        );
    }
}

unsigned modulo(int value, unsigned m) {
    int mod = value % (int)m;
    if (mod < 0) {
        mod += m;
    }
    return mod;
}