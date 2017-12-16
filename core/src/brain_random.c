#include "brain_random.h"

#define BRAIN_MASK_SIZE (8 * sizeof(BrainUint))
#define BRAIN_ACTIVATION(mask,j,k) ((mask[j] >> k) & 0x00000001)

struct RandomMask
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
    BrainRandomMask _random_mask = (BrainRandomMask)calloc(1, sizeof(RandomMask));
    BrainUint i = 0;

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

    _random_mask->_mask       = (BrainUint *) calloc(_random_mask->_mask_size, sizeof(BrainUint));
    _random_mask->_mask_max = (BrainUint *)calloc(_random_mask->_mask_size, sizeof(BrainUint));
    _random_mask->_mask_node  = (BrainUint *) calloc(number_of_elements, sizeof(BrainUint));
    _random_mask->_mask_slot  = (BrainUint *) calloc(number_of_elements, sizeof(BrainUint));

    memset(_random_mask->_mask_max, UINT_MAX, _random_mask->_mask_size * sizeof(BrainUint));
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
    if (random_mask != NULL)
    {
        if (random_mask->_mask != NULL)
        {
            free(random_mask->_mask);
        }

        if (random_mask->_mask_slot != NULL)
        {
            free(random_mask->_mask_slot);
        }

        if (random_mask->_mask_node != NULL)
        {
            free(random_mask->_mask_node);
        }
        
        if (random_mask->_mask_max != NULL)
        {
            free(random_mask->_mask_max);
        }

        free(random_mask);
        random_mask = NULL;
    }
}

void
generate_random_mask(BrainRandomMask random_mask)
{
    if (random_mask != NULL)
    {
        BrainUint i = 0;

        for (i = 0; i < random_mask->_mask_size; ++i)
        {
            random_mask->_mask[i] = (BrainUint)BRAIN_RAND_RANGE(0, random_mask->_mask_max[i]);
        }
    }
}

void
generate_unit_mask(BrainRandomMask random_mask)
{
    if (random_mask)
    {
        memset(random_mask->_mask, UINT_MAX, random_mask->_mask_size*sizeof(BrainUint));
    }
}


BrainBool
get_random_state(const BrainRandomMask random_mask, const BrainUint index)
{
    BrainBool ret = BRAIN_TRUE;

    if ((random_mask != NULL) &&
        (index < random_mask->_number_of_elements))
    {
        const BrainUint j = random_mask->_mask_node[index];
        const BrainUint k = random_mask->_mask_slot[index];

        ret = BRAIN_ACTIVATION(random_mask->_mask, j, k);
    }

    return ret;
}
