#ifndef BRAIN_ACTIVATION_H
#define BRAIN_ACTIVATION_H

#include "brain_types.h"
#include <math.h>

PtrFunc             activation(const BrainActivationType type);
PtrFunc             derivative(const BrainActivationType type);
BrainActivationType get_activation_type(BrainString activation_type_name);

#endif /* BRAIN_ACTIVATION_H */
