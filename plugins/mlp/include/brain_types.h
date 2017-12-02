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

/**
 * \enum BrainActivationType
 * \brief enumeration to choose neurons activation function
 */
typedef enum BrainActivationType
{
    Invalid_Activation, /*!< Invalid activation  */
    Identity,           /*!< Identity activation */
    Sigmoid,            /*!< sigmoid activation  */
    TanH,               /*!< tanh activation     */
    ArcTan,             /*!< arctan activation   */
    SoftPlus,           /*!< softplus activation */
    Sinusoid,           /*!< sinusoid activation */

    First_Activation = Invalid_Activation,
    Last_Activation  = Sinusoid
} BrainActivationType;

/**
 * \enum BrainCostFunctionType
 * \brief enumeration to choose network cost function
 */
typedef enum BrainCostFunctionType
{
    Invalid_CostFunction, /*!< invalid cost function      */
    Quadratic,            /*!< Quadratic cost function    */
    CrossEntropy,         /*!< CrossEntropy cost function */

    First_CostFunction = Invalid_CostFunction,
    Last_CostFunction  = CrossEntropy
} BrainCostFunctionType;

/**
 * \enum BrainLearningType
 * \brief enumeration to choose network learning process
 */
typedef enum BrainLearningType
{
    Invalid_Learning, /*!< Invalid learning         */
    BackPropagation,  /*!< Backpropagation learning */
    Resilient,        /*!< Resilient learning       */

    First_Learning = Invalid_Learning,
    Last_Learning  = Resilient
} BrainLearningType;

/**
 * \brief Define a BrainSignal using single or double precision
 */
#if BRAIN_ENABLE_DOUBLE_PRECISION
typedef BrainDouble BrainReal;
#else
typedef BrainFloat BrainReal;
#endif
typedef BrainReal* BrainSignal;

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
 * \brief opaque pointer on Network struct
 */
typedef struct Network* BrainNetwork;
/**
 * \brief function pointer on an activation function
 *
 * It let neurons use several activation function and
 * automatically compute the activation an it's derivation
 *
 * \param value dot product of input vector and weight vector of a neuron
 * \return the value of the activation
 */
typedef BrainReal (*ActivationPtrFunc)(const BrainReal value);
/**
 * \brief function pointer on an cost function
 *
 * it let the network use several cost function and automatically compute
 * the derivative
 *
 * \param output the output of a neuron
 * \param desired the desired output of a neuron
 * \return the value of the cost function
 */
typedef BrainReal (*CostPtrFunc)(const BrainReal output, const BrainReal desired);
/**
 * \brief function pointer to update neuron weights
 *
 * \param neuron a BrainNeuron
 * \param loss   the total output error
 */
typedef void (*LearningPtrFunc)(BrainNeuron neuron, const BrainReal loss);
#endif /* BRAIN_TYPES_H */
