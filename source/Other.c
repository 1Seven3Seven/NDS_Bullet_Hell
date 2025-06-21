#include "Other.h"

#include <nds.h>

#include <math.h>
#include <stdlib.h>

#include "GFX.h"

int RectangleCollision(int rect1[4], int rect2[4])
{
    // If rect1_left < rect2_right
    if (RectangleGetLeft(rect1) < RectangleGetRight(rect2))
    {
        // If rect1_right > rect2_left
        if (RectangleGetRight(rect1) > RectangleGetLeft(rect2))
        {
            // If rect1_bottom > rect2_top
            if (RectangleGetBottom(rect1) > RectangleGetTop(rect2))
            {
                // If rect1_top < rect2_bottom
                if (RectangleGetTop(rect1) < RectangleGetBottom(rect2))
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int RectangleGetLeft(int rect[4])
{
    return rect[0];
}

int RectangleGetRight(int rect[4])
{
    return rect[0] + rect[2];
}

int RectangleGetTop(int rect[4])
{
    return rect[1];
}

int RectangleGetBottom(int rect[4])
{
    return rect[1] + rect[3];
}

float GetAngleFromOriginTo(const float x, const float y)
{
    float angle;

    if (x != 0) angle = atanf(fabsf(y) / fabsf(x));
    else
    {
        if (y < 0) angle = (float) M_PI / 2;
        else angle = 3 * (float) M_PI / 2;
    }

    if (x < 0 && y < 0) angle = (float) M_PI - angle;
    else if (x < 0 && y >= 0) angle += (float) M_PI;
    else if (x > 0 && y > 0) angle = 2 * (float) M_PI - angle;

    return angle;
}

void GetUnitVectorFromAngle(const float angle, float vector_array[2])
{
    vector_array[0] = cosf(angle);
    vector_array[1] = -sinf(angle);
}

void GetVectorFromTo(
    const float x1,
    const float y1,
    const float x2,
    const float y2,
    const float magnitude,
    float vector_array[2])
{
    // Vector time
    vector_array[0] = x2 - x1;
    vector_array[1] = y2 - y1;

    // Magnitude
    const float old_magnitude = sqrtf(vector_array[0] * vector_array[0] + vector_array[1] * vector_array[1]);

    // Make length be magnitude
    vector_array[0] *= magnitude / old_magnitude;
    vector_array[1] *= magnitude / old_magnitude;
}

void ShuffleIntArray(int array[], const int array_len)
{
    if (array_len > 1)
    {
        for (int i = array_len - 1; i > 0; i--)
        {
            const int j = rand() % (i + 1);

            const int tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
        }
    }
}

void HideEverySprite()
{
    for (int i = 0; i < 128; i++)
    {
        oamSetHidden(
            &oamMain,
            i,
            true
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
