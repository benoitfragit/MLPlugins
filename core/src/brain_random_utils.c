#include "brain_random_utils.h"
#include "brain_memory_utils.h"
#include <math.h>

#define BRAIN_MASK_SIZE (8 * sizeof(BrainUint))
#define BRAIN_ACTIVATION(mask,j,k) ((mask[j] >> k) & 0x00000001)

typedef struct RandomMask
{
    BrainUint  _number_of_elements;
    BrainUint  _mask_size;
    BrainUint* _mask_node;
    BrainUint* _mask_slot;
    BrainUint* _mask_max;
    BrainUint* _mask;
} RandomMask;

BrainRandomMask
new_random_mask(const BrainUint number_of_elements)
{
    BrainUint i = 0;
    BrainRandomMask _random_mask = NULL;

    BRAIN_NEW(_random_mask, RandomMask, 1);

    _random_mask->_number_of_elements = number_of_elements;
    _random_mask->_mask_size  = (BrainUint)(number_of_elements / BRAIN_MASK_SIZE);
    if (_random_mask->_mask_size == 0)
    {
        _random_mask->_mask_size = 1;
    }
    else
    {
        if ((number_of_elements - _random_mask->_mask_size * BRAIN_MASK_SIZE) >= 1)
        {
            ++_random_mask->_mask_size;
        }
    }

    BRAIN_NEW(_random_mask->_mask,      BrainUint, _random_mask->_mask_size);
    BRAIN_NEW(_random_mask->_mask_max,  BrainUint, _random_mask->_mask_size);
    BRAIN_NEW(_random_mask->_mask_node, BrainUint, number_of_elements);
    BRAIN_NEW(_random_mask->_mask_slot, BrainUint, number_of_elements);

    BRAIN_SET(_random_mask->_mask_max, UINT_MAX, BrainUint, _random_mask->_mask_size);
    if (_random_mask->_mask_size == 1)
    {
        _random_mask->_mask_max[0] = pow(2, number_of_elements) - 1;
    }
    else
    {
        if (_random_mask->_mask_size > 1)
        {
            _random_mask->_mask_max[_random_mask->_mask_size - 1] = pow(2, number_of_elements - _random_mask->_mask_size * BRAIN_MASK_SIZE) - 1;
        }
    }

    for (i = 0; i < number_of_elements; ++i)
    {
        _random_mask->_mask_node[i] = i / BRAIN_MASK_SIZE;
        _random_mask->_mask_slot[i] = i % BRAIN_MASK_SIZE;
    }

    return _random_mask;
}

void
delete_random_mask(BrainRandomMask random_mask)
{
    if (BRAIN_ALLOCATED(random_mask))
    {
        BRAIN_DELETE(random_mask->_mask);
        BRAIN_DELETE(random_mask->_mask_slot);
        BRAIN_DELETE(random_mask->_mask_node);
        BRAIN_DELETE(random_mask->_mask_max);
        BRAIN_DELETE(random_mask);
    }
}

BrainUint
generate_random_mask(BrainRandomMask random_mask)
{
    BrainUint ret = 0;

    if (BRAIN_ALLOCATED(random_mask))
    {
        BrainUint i = 0;
        BrainUint j = 0;

        for (i = 0; i < random_mask->_mask_size; ++i)
        {
            random_mask->_mask[i] = (BrainUint)BRAIN_RAND_RANGE(0, random_mask->_mask_max[i]);

            for (j = 0; j < BRAIN_MASK_SIZE; ++j)
            {
                ret += BRAIN_ACTIVATION(random_mask->_mask, i, j);
            }
        }
    }

    return ret;
}

void
generate_unit_mask(BrainRandomMask random_mask)
{
    if (BRAIN_ALLOCATED(random_mask))
    {
        BRAIN_SET(random_mask->_mask, UINT_MAX, BrainUint, random_mask->_mask_size);
    }
}


BrainBool
get_random_state(const BrainRandomMask random_mask, const BrainUint index)
{
    BrainBool ret = BRAIN_TRUE;

    if (BRAIN_ALLOCATED(random_mask) &&
        (index < random_mask->_number_of_elements))
    {
        const BrainUint j = random_mask->_mask_node[index];
        const BrainUint k = random_mask->_mask_slot[index];

        ret = BRAIN_ACTIVATION(random_mask->_mask, j, k);
    }

    return ret;
}
