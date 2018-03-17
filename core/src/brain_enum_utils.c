#include "brain_enum_utils.h"
#include "brain_memory_utils.h"

BrainUint
get_enum_values(const BrainString* labels,
                const BrainUint first,
                const BrainUint last,
                BrainString val)
{
    BrainUint ret = last;
    if (BRAIN_ALLOCATED(labels) &&
        BRAIN_ALLOCATED(val))
    {
        BrainUint i = first;
        for (i = first; i < last; ++i)
        {
            if (BRAIN_ALLOCATED(labels[i]) &&
                !strcmp(labels[i], val))
            {
                ret = i;
                break;
            }
        }
    }
    return ret;
}
