/**
 * \file brain_random.h
 * \brief Define the API to build and use a random number generator
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to build a Layer
 */
#ifndef BRAIN_RANDOM_H
#define BRAIN_RANDOM_H

#include "brain_core_types.h"

#define BRAIN_RANDOM_INITIALIZATION srand(time(NULL));
#define BRAIN_RAND_UNIT (BrainDouble)rand()/(1.0 + (BrainDouble)RAND_MAX)
#define BRAIN_RAND_RANGE(min, max) BRAIN_RAND_UNIT * (BrainDouble)(max - min) + (BrainDouble)min

#endif /* BRAIN_RANDOM_H */
