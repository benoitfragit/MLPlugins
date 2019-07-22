/**
 * \file mlp_neuron.h
 * \brief Define the API to create an MLPNeuron
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to create aa MLPNeuron
 */
#ifndef MLP_NEURON_H
#define MLP_NEURON_H

#include "mlp_types.h"

/**
 * \fn MLPNeuron new_neuron(BrainSignal in,
 *                            const BrainUint number_of_inputs,
 *                            BrainSignal out,
 *                            BrainSignal errors)
 * \brief method to build a neuron
 *
 * \param in               a input BrainSignal
 * \param number_of_inputs input_signal_size
 * \param out            a pointer to a BrainReal owned by the MLPLayer
 * \param errors an array owned by the MLPLayer to update weights
 * \return a MLPNeuron or NULL if it failed
 */
MLPNeuron new_neuron(BrainSignal in,
                       const BrainUint     number_of_inputs,
                       BrainSignal         out,
                       BrainSignal         errors);
/**
 * \fn void delete_neuron(MLPNeuron neuron)
 * \brief free all MLPNeuron memory
 *
 * \param neuron a MLPNeuron
 */
void        delete_neuron              (MLPNeuron       neuron);
/**
 * \fn void activate_neuron(MLPNeuron neuron, const BrainBool   is_activated)
 * \brief activate the input neuron
 *
 * \param neuron            a MLPNeuron
 * \param is_activated is this neuron activated
 */
void activate_neuron(MLPNeuron neuron, const BrainBool is_activated);
/**
 * \fn BrainUint get_neuron_number_of_input(const MLPNeuron neuron)
 * \brief retrieve the number of input
 *
 * \param neuron the Neuron
 * \return the number of input
 */
BrainUint get_neuron_number_of_input(const MLPNeuron neuron);
/**
 * \fn BrainReal get_neuron_weight(const MLPNeuron neuron, const BrainUint index)
 * \brief retrieve a neuron weight
 *
 * \param neuron the Neuron
 * \param index the weight inde
 * \return the neuron weight
 */
BrainReal get_neuron_weight(const MLPNeuron neuron, const BrainUint index);
/**
 * \fn void set_neuron_activation( MLPNeuron neuron, BrainString name);
 * \brief customize neural network parameters
 *
 * \param neuron    The MLPNeuron to train
 * \param name      The function name
 */
void set_neuron_activation(MLPNeuron neuron, BrainString name);
/**
 * \fn void update_neuron(MLPNeuron reuron,
 *                        BrainReal minibatch_size,
 *                        BrainReal learning_rate,
 *                        BrainReal momemtum )
 * \brief apply correction to a neuron to reduce the total error
 *
 * \param neuron a MLPNeuron
 * \param minibatch_size size of the mini batch
 */
void update_neuron(MLPNeuron neuron,
                    BrainReal learning_rate,
                    BrainReal momentum);
/**
 * \fn void deserialize_neuron(MLPNeuron neuron, Context context)
 * \brief deserialize a neuron from a context
 *
 * \param neuron a MLPNeuron
 * \param context an xml context
 */
void deserialize_neuron(MLPNeuron neuron, Context context);
/**
 * \fn void serialize_neuron(MLPNeuron neuron, Writer writer)
 * \brief serialize a neuron
 *
 * \param neuron the neuron to be serialized
 * \param writer the xml writer
 */
void serialize_neuron(MLPNeuron neuron, Writer writer);
/**
 * \fn void backpropagate_neuron_gradient(MLPNeuron neuron, const BranReal loss);
 * \brief accummulate gradient over minibatch loss input and backpropagate gradient based error
 *
 * \param neuron the neuron
 * \param loss the error
 */
void backpropagate_neuron_gradient(MLPNeuron neuron, const BrainReal loss);
#endif /* MLP_NEURON_H */
