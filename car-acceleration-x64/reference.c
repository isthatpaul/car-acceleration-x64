#include "acceleration.h"

#include <math.h>

static int acceleration_to_int(float initial_velocity,
                               float final_velocity,
                               float time_seconds)
{
    const float kilometers_per_hour_to_meters_per_second = 1000.0f / 3600.0f;
    const float acceleration =
        ((final_velocity - initial_velocity) *
         kilometers_per_hour_to_meters_per_second) /
        time_seconds;

    /* lrintf follows the default round-to-nearest mode used by cvtss2si. */
    return (int)lrintf(acceleration);
}

void acceleration_c(const float *matrix, int rows, int *output)
{
    for (int row = 0; row < rows; ++row) {
        const float *values = matrix + (row * 3);
        output[row] = acceleration_to_int(values[0], values[1], values[2]);
    }
}
