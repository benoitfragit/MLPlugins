#include "brain_random.h"
#include <time.h>
#include <stdlib.h>

#define RAND_SCALE (BrainDouble)rand()/(BrainDouble)RAND_MAX

void
initialize_random_generator()
{
    srand(time(NULL));
}

BrainDouble
get_random_double_value()
{
    // return a random double value between 0 and 1
    return RAND_SCALE;
}

BrainDouble
get_random_double_value_in_range(const BrainDouble min,
                                 const BrainDouble max)
{
    // return a random double value between min and max
    return RAND_SCALE * (max - min) + min;
}

BrainInt
get_random_int_value_in_range(const BrainInt min,
                              const BrainInt max)
{
    return RAND_SCALE * (BrainDouble)(max - min) + min;
}

BrainUint
get_random_uint_value_in_range(const BrainUint min,
                               const BrainUint max)
{
    return RAND_SCALE * (BrainDouble)(max - min) + min;
}
