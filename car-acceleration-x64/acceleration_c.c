#include "acceleration.h"

#include <math.h>

void acceleration_c(const float *matrix, int rows, int *output)
{
    for (int row = 0; row < rows; ++row) {
        const float *car = matrix + (row * 3);
        const float kilometers_per_hour_to_meters_per_second = 1000.0f / 3600.0f;
        const float acceleration =
            ((car[1] - car[0]) * kilometers_per_hour_to_meters_per_second) / car[2];

        /* lrintf matches cvtss2si's default round-to-nearest behavior. */
        output[row] = (int)lrintf(acceleration);
    }
}
