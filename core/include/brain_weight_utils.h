#ifndef BRAIN_WEIGHT_UTILS
#define BRAIN_WEIGHT_UTILS
#include "brain_core_types.h"

BrainWeight new_weight(BrainReal);

void update_weight(BrainWeight, BrainReal, BrainReal);

BrainReal get_weight(BrainWeight);

void set_weight(BrainWeight, BrainReal);

void accumulate_gradient(BrainWeight, BrainReal, BrainReal);

#endif /* BRAIN_WEIGHT_UTILS  */
