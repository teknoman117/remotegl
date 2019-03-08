#ifndef TIMESPEC_H_
#define TIMESPEC_H_

#include <time.h>

/**
 * Get the delta between two timespec structures
 *
 * @param start the starting time (the earlier timespec)
 * @param end the ending time (the later timespec)
 * @return a timespec struct containing the delta between the source timespecs
 */
struct timespec timespec_diff(struct timespec start, struct timespec end);

/**
 * Add two timespec structures
 *
 * @param a a timespec struct
 * @param b a timespec struct
 * @return a timespec struct equal to a plus b with proper overflow handling
 */
struct timespec timespec_add(struct timespec a, struct timespec b);

/**
 * Convert a timespec struct to a float
 *
 * @param a a timespec struct
 * @return floating point value representing the value in seconds of the timespec struct
 */
float timespec_to_float(struct timespec a);

#endif
