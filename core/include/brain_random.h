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
/**
 * \fn void initialize_random_generator()
 * \brief initialize the random number generator
 */
void initialize_random_generator();
/**
 * \fn BrainDouble get_random_double_value()
 * \brief get a random double value between 0 and 1
 *
 * \return a random value between 0 and 1
 */
BrainDouble get_random_double_value();
/**
 * \fn BrainDouble get_random_double_value_in_range(const BrainDouble min, const BrainDouble max)
 * \brief get a random double value between min and max
 * \param min min value
 * \param max max value
 * \return a random value between min and max
 */
BrainDouble get_random_double_value_in_range(const BrainDouble min, const BrainDouble max);
/**
 * \fn BrainInt get_random_int_value_in_range(const BrainInt min, const BrainInt max)
 * \brief get a random int value between min and max
 * \param min min value
 * \param max max value
 * \return a random value between min and max
 */
BrainInt    get_random_int_value_in_range(const BrainInt min, const BrainInt max);

#endif /* BRAIN_RANDOM_H */
