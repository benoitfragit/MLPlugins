/**
 * \file brain_neuron.h
 * \brief Define the API to create an BrainNeuron
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to create aa BrainNeuron
 */
#ifndef BRAIN_NEURON_H
#define BRAIN_NEURON_H

#include "brain_types.h"

/**
 * \fn BrainNeuron new_neuron(BrainSignal in,
 *                            const BrainUint number_of_inputs,
 *                            BrainSignal out,
 *                            BrainSignal errors)
 * \brief method to build a neuron
 *
 * \param in               a input BrainSignal
 * \param number_of_inputs input_signal_size
 * \param out            a pointer to a BrainDouble owned by the BrainLayer
 * \param errors an array owned by the BrainLayer to update weights
 * \return a BrainNeuron or NULL if it failed
 */
BrainNeuron new_neuron(BrainSignal in,
                       const BrainUint     number_of_inputs,
                       BrainSignal         out,
                       BrainSignal         errors);
/**
 * \fn void delete_neuron(BrainNeuron neuron)
 * \brief free all BrainNeuron memory
 *
 * \param neuron a BrainNeuron
 */
void        delete_neuron              (BrainNeuron       neuron);
/**
 * \fn void activate_neuron(BrainNeuron neuron, const BrainBool   is_an_hidden_unit)
 * \brief activate the input neuron
 *
 * \param neuron            a BrainNeuron
 * \param is_an_hidden_unit is this neuron in an hidden layer
 */
void activate_neuron(BrainNeuron neuron, const BrainBool is_an_hidden_unit);
/**
 * \fn BrainUint get_neuron_number_of_input(const BrainNeuron neuron)
 * \brief retrieve the number of input
 *
 * \param neuron the Neuron
 * \return the number of input
 */
BrainUint get_neuron_number_of_input(const BrainNeuron neuron);
/**
 * \fn BrainDouble get_neuron_bias(const BrainNeuron neuron)
 * \brief retrieve neuron bias
 *
 * \param neuron the Neuron
 * \return the neuron bias
 */
BrainDouble get_neuron_bias(const BrainNeuron neuron);
/**
 * \fn BrainDouble get_neuron_weight(const BrainNeuron neuron, const BrainUint index)
 * \brief retrieve a neuron weight
 *
 * \param neuron the Neuron
 * \param index the weight inde
 * \return the neuron weight
 */
BrainDouble get_neuron_weight(const BrainNeuron neuron, const BrainUint index);
/**
 * \fn void configure_neuron_with_context( BrainNeuron neuron, Context context);
 * \brief customize neural network parameters
 *
 * \param neuron    The BrainNeuron to train
 * \param context   The context
 */
void configure_neuron_with_context(BrainNeuron neuron, Context context);
/**
 * \fn void neuron_learning(BrainNeuron reuron, const BrainDouble loss)
 * \brief apply correction to a neuron to reduce the total error
 *
 * \param neuron a BrainNeuron
 * \param error The error procuded by this neuron
 */
void neuron_learning(BrainNeuron neuron, const BrainDouble loss);
/**
 * \fn void deserialize_neuron(BrainNeuron neuron, Context context)
 * \brief deserialize a neuron from a context
 *
 * \param neuron a BrainNeuron
 * \param context an xml context
 */
void deserialize_neuron(BrainNeuron neuron, Context context);
/**
 * \fn void serialize_neuron(BrainNeuron neuron, Writer writer)
 * \brief serialize a neuron
 *
 * \param neuron the neuron to be serialized
 * \param writer the xml writer
 */
void serialize_neuron(BrainNeuron neuron, Writer writer);
#endif /* BRAIN_NEURON_H */
