/**
 * \file brain_random.h
 * \brief Define the API to build and use a random number generator
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to build a Layer
 */
#ifndef BRAIN_RANDOM_UTILS_H
#define BRAIN_RANDOM_UTILS_H

#include "brain_core_types.h"

#define BRAIN_RANDOM_INITIALIZATION srand(time(NULL));
#define BRAIN_RAND_UNIT (BrainDouble)rand()/((BrainDouble)RAND_MAX)
#define BRAIN_RAND_RANGE(min, max) BRAIN_RAND_UNIT * (BrainDouble)(max - min) + (BrainDouble)min

BrainRandomMask new_random_mask     (const BrainUint number_of_elements);
void            delete_random_mask  (BrainRandomMask random_mask);
BrainUint       generate_random_mask(BrainRandomMask random_mask);
void            generate_unit_mask  (BrainRandomMask random_mask);
BrainBool       get_random_state    (const BrainRandomMask random_mask, const BrainUint index);

#endif /* BRAIN_RANDOM_UTILS_H */
