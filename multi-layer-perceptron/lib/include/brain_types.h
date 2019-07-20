/**
 * \file brain_types.h
 * \brief Define all types
 * \author Benoit F.
 * \date 11 decembre 2016
 *
 * This file contains all predefines types used in this library
 */
#ifndef BRAIN_TYPES_H
#define BRAIN_TYPES_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "brain_core_types.h"

#define __BRAIN_VISIBLE__ __attribute__((visibility("default")))

/**
 * \brief opaque pointer on Data struct
 */
typedef struct Data*    BrainData;
/**
 * \brief opaque pointer on Neuron struct
 */
typedef struct Neuron*  BrainNeuron;
/**
 * \brief opaque pointer on Layer struct
 */
typedef struct Layer*   BrainLayer;
/**
 * \brief opaque pointer to a Trainer
 */
 typedef struct Trainer* BrainTrainer;
#endif /* BRAIN_TYPES_H */
