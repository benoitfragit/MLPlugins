/** \internal
 * \file brain_activation.h
 * \brief Define the API to retrieve Activation functions
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to get an Activation function
 */
#ifndef BRAIN_ACTIVATION_H
#define BRAIN_ACTIVATION_H

#include "brain_types.h"
#include <math.h>

/**
 * \fn PtrFunc activation(const BrainActivationType type)
 * \brief get an activation function
 *
 * \param type BrainActivationType to get the corresponding Activation function
 * \return an activation function or NULL
 */
ActivationPtrFunc   activation(const BrainActivationType type);
/**
 * \fn PtrFunc derivative(const BrainActivationType type)
 * \brief get an activation derivative function
 *
 * \param type BrainActivationType to get the corresponding Activation derivative function
 * \return a activation derivative function or NULL
 */
ActivationPtrFunc   derivative(const BrainActivationType type);
/**
 * \fn BrainActivationType get_activation_type(BrainString activation_type_name)
 * \brief get a BrainActivationType according to its raw name
 *
 * \param activation_type_name Raw name of the activation type
 * \return a BrainActivationType
 */
BrainActivationType get_activation_type(BrainString activation_type_name);

#endif /* BRAIN_ACTIVATION_H */
