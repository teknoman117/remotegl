#include "timespec.h"

/**
 * Get the delta between two timespec structures
 *
 * @param start the starting time (the earlier timespec)
 * @param end the ending time (the later timespec)
 * @return a timespec struct containing the delta between the source timespecs
 */
struct timespec timespec_diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0)
    {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    }
    else
    {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

/**
 * Add two timespec structures
 *
 * @param a a timespec struct
 * @param b a timespec struct
 * @return a timespec struct equal to a plus b with proper overflow handling
 */
struct timespec timespec_add(struct timespec a, struct timespec b)
{
    struct timespec result =
    {
        a.tv_sec + b.tv_sec,
        a.tv_nsec + b.tv_nsec
    };

    result.tv_sec += result.tv_nsec / 1000000000;
    result.tv_nsec %= 1000000000;
    return result;
}

/**
 * Convert a timespec struct to a float
 *
 * @param a a timespec struct
 * @return floating point value representing the value in seconds of the timespec struct
 */
float timespec_to_float(struct timespec a)
{
    return ((float) a.tv_sec) + ((float) a.tv_nsec) / 1000000000.f;
}
