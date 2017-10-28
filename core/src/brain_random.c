#include "brain_random.h"
#include <time.h>
#include <stdlib.h>

void
initialize_random_generator()
{
    srand(time(NULL));
}

BrainDouble
get_random_double_value()
{
    // return a random double value between 0 and 1
    return (BrainDouble)rand() / (BrainDouble)RAND_MAX;
}

BrainDouble
get_random_double_value_in_range(const BrainDouble min,
                                 const BrainDouble max)
{
    // return a random double value between min and max
    return get_random_double_value() * (max - min) + min;
}

BrainInt
get_random_int_value_in_range(const BrainInt min,
                              const BrainInt max)
{
    return rand() % (BrainUint)(max - min + 1) + min;
}
