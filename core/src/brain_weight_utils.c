#include "brain_weight_utils.h"
#include "brain_random_utils.h"
#include "brain_memory_utils.h"

typedef struct Weight
{
    BrainReal _w;         /*!< An array of weight                   */
    BrainReal _gradient;  /*!< weights gradient value               */
    BrainReal _delta;     /*!< weights delta value                  */
} Weight;

BrainWeight
new_weight(BrainReal random_value_limit)
{
    BrainWeight _weight = NULL;

    BRAIN_NEW(_weight, Weight, 1);

    _weight->_w = (BrainReal)BRAIN_RAND_RANGE(-random_value_limit, random_value_limit);
    _weight->_delta = 0.;
    _weight->_gradient = 0.;

    return _weight;
}

void
update_weight(BrainWeight weight,
              BrainReal learning_rate,
              BrainReal momentum)
{
    if (BRAIN_ALLOCATED(weight))
    {
        BrainReal delta = learning_rate * weight->_gradient - momentum * weight->_delta;

        weight->_w -= delta;
        weight->_gradient = 0.;
        weight->_delta = delta;
    }
}

BrainReal
get_weight(BrainWeight weight)
{
    BrainReal value = 0.;
    if (BRAIN_ALLOCATED(weight))
    {
        value = weight->_w;
    }
    return value;
}

void
set_weight(BrainWeight weight, BrainReal value)
{
    if (BRAIN_ALLOCATED(weight))
    {
        weight->_w = value;
        weight->_delta = 0.;
        weight->_gradient = 0.;
    }
}

void
accumulate_gradient(BrainWeight weight, BrainReal gradient, BrainReal in)
{
    if (BRAIN_ALLOCATED(weight))
    {
        weight->_gradient += gradient * in;
    }
}
