/**
 * \file mlp_types.h
 * \brief Define all types
 * \author Benoit F.
 * \date 11 decembre 2016
 *
 * This file contains all predefines types used in this library
 */
#ifndef MLP_TYPES_H
#define MLP_TYPES_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "brain_core_types.h"

/**
 * \brief opaque pointer on Data struct
 */
typedef struct Data*    MLPData;
/**
 * \brief opaque pointer on Neuron struct
 */
typedef struct Neuron*  MLPNeuron;
/**
 * \brief opaque pointer on Layer struct
 */
typedef struct Layer*   MLPLayer;
/**
 * \brief opaque pointer on Network struct
 */
typedef struct Network* MLPNetwork;
/**
 * \brief opaque pointer to a Trainer
 */
typedef struct Trainer* MLPTrainer;

#endif /* MLP_TYPES_H */
