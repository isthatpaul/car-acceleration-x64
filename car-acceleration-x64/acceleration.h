#ifndef ACCELERATION_H
#define ACCELERATION_H

/*
 * Each matrix row contains:
 *   [initial velocity in km/h, final velocity in km/h, time in seconds]
 */
void acceleration_c(const float *matrix, int rows, int *output);
void acceleration_asm(const float *matrix, int rows, int *output);

#endif
