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

#include "brain_xml_utils.h"

/**
 * \def BRAIN_TRUE
 */
#define BRAIN_TRUE  1
/**
 * \def BRAIN_FALSE
 */
#define BRAIN_FALSE 0

typedef unsigned char BrainBool;
typedef int           BrainInt;
typedef unsigned int  BrainUint;
typedef float         BrainFloat;
typedef double        BrainDouble;
typedef const char*   BrainString;
typedef char          BrainChar;
typedef BrainDouble*  BrainSignal;
typedef BrainDouble*  BrainWeight;

/**
 * \enum BrainResult
 * \brief Return value for the train method.
 */
typedef enum BrainResult
{
    BRAIN_FAILED  = 0, /*!< Training failed  */
    BRAIN_SUCCESS = 1  /*!< Training succeed */
} BrainResult;

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
 * \brief function pointer on an activation function
 *
 * It let neurons use several activation function and
 * automatically compute the activation an it's derivation
 *
 * \param value dot product of input vector and weight vector of a neuron
 * \return the value of the activation
 */
typedef BrainDouble (*PtrFunc)(const BrainDouble value);
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
typedef BrainDouble (*CostPtrFunc)(const BrainDouble output,
                                   const BrainDouble desired);
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

#endif /* BRAIN_TYPES_H */
