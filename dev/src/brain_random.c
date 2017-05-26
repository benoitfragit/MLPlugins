#include "brain_random.h"
#include <time.h>
#include <stdlib.h>

static BrainBool _is_random_generator_initialized = BRAIN_FALSE;

static void
initialize_random_generator()
{
    srand(time(NULL));

    _is_random_generator_initialized = BRAIN_TRUE;
}

BrainDouble
get_random_double_value()
{
    if (_is_random_generator_initialized == BRAIN_FALSE)
    {
        initialize_random_generator();
    }

    // return a random double value between 0 and 1
    return (BrainDouble)rand() / (BrainDouble)RAND_MAX;
}

BrainDouble
get_random_double_value_in_range(const BrainDouble min,
                                 const BrainDouble max)
{
    if (_is_random_generator_initialized == BRAIN_FALSE)
    {
        initialize_random_generator();
    }

    // return a random double value between min and max
    return get_random_double_value() * (max - min) + min;
}

BrainInt
get_random_int_value_in_range(const BrainInt min,
                              const BrainInt max)
{
    if (_is_random_generator_initialized == BRAIN_FALSE)
    {
        initialize_random_generator();
    }

    return rand() % (BrainUint)(max - min + 1) + min;
}
