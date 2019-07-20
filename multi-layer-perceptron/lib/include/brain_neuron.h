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
 * \param out            a pointer to a BrainReal owned by the BrainLayer
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
 * \fn void activate_neuron(BrainNeuron neuron, const BrainBool   is_activated)
 * \brief activate the input neuron
 *
 * \param neuron            a BrainNeuron
 * \param is_activated is this neuron activated
 */
void activate_neuron(BrainNeuron neuron, const BrainBool is_activated);
/**
 * \fn BrainUint get_neuron_number_of_input(const BrainNeuron neuron)
 * \brief retrieve the number of input
 *
 * \param neuron the Neuron
 * \return the number of input
 */
BrainUint get_neuron_number_of_input(const BrainNeuron neuron);
/**
 * \fn BrainReal get_neuron_weight(const BrainNeuron neuron, const BrainUint index)
 * \brief retrieve a neuron weight
 *
 * \param neuron the Neuron
 * \param index the weight inde
 * \return the neuron weight
 */
BrainReal get_neuron_weight(const BrainNeuron neuron, const BrainUint index);
/**
 * \fn void set_neuron_activation( BrainNeuron neuron, BrainString name);
 * \brief customize neural network parameters
 *
 * \param neuron    The BrainNeuron to train
 * \param name      The function name
 */
void set_neuron_activation(BrainNeuron neuron, BrainString name);
/**
 * \fn void update_neuron(BrainNeuron reuron,
 *                        BrainReal minibatch_size,
 *                        BrainReal learning_rate,
 *                        BrainReal momemtum )
 * \brief apply correction to a neuron to reduce the total error
 *
 * \param neuron a BrainNeuron
 * \param minibatch_size size of the mini batch
 */
void update_neuron(BrainNeuron neuron,
                    BrainReal learning_rate,
                    BrainReal momentum);
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
/**
 * \fn void backpropagate_neuron_gradient(BrainNeuron neuron, const BranReal loss);
 * \brief accummulate gradient over minibatch loss input and backpropagate gradient based error
 *
 * \param neuron the neuron
 * \param loss the error
 */
void backpropagate_neuron_gradient(BrainNeuron neuron, const BrainReal loss);
#endif /* BRAIN_NEURON_H */
