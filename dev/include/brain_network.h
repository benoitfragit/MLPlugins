/**
 * \file brain_network.h
 * \brief Define the API to create an control a Neural network
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to create and control a NeuralNetwork
 */

#ifndef BRAIN_NETWORK_H
#define BRAIN_NETWORK_H

#include "brain_types.h"

/**
 * \fn BrainNetwork new_network(const BrainUint signal_input_length,
 *                              const BrainUint number_of_layers,
 *                              const BrainUint *neuron_per_layers)
 *
 * \brief Fonction to create a BrainNetwork
 *
 * \param signal_input_length Size of the input signal
 * \param number_of_layers    Number of layer in this network
 * \param neuron_per_layers   Array containing neumber of neurons per layer
 *
 * \return a new allocated BrainNetwork or NULL if it failed
 */
BrainNetwork new_network(const BrainUint             signal_input_length,
                         const BrainUint             number_of_layers,
                         const BrainUint             *neuron_per_layers);
/**
 * \fn void delete_network(BrainNetwork network)
 * \brief deallocate a BrainNetwork
 *
 * \param network BrainNetwork to deallocate
 */
void delete_network(BrainNetwork network);
/**
 * \fn void feedforward(BrainNetwork network, const BrainUint number_of_input, const BrainSignal in)
 * \brief propagate an input signal from the input signal to the output layer
 *
 * \param network the network to feed
 * \param number_of_input the length of the input signal
 * \param in the input signal
 */
void feedforward(BrainNetwork      network,
                 const BrainUint   number_of_input,
                 const BrainSignal in);
/**
 * \fn BrainSignal get_network_output(const BrainNetwork network)
 * \brief get the output of the network
 *
 * \param network the BrainNetwork
 * \return get the output signal
 */
BrainSignal get_network_output(const BrainNetwork network);
/**
 * \fn BrainLayer get_network_layer(const BrainNetwork network, const BrainUint index)
 * \brief retrieve the input layer
 *
 * \param network      the BrainNetwork to train
 * \param index         index of the requested layer
 *
 * \return a BrainLayer
 */
BrainLayer get_network_layer(const BrainNetwork network, const BrainUint index);
/**
 * \fn BrainDouble backpropagate(BrainNetwork network, const BrainUint number_of_output, const BrainSignal desired)
 * \brief backpropagate an input signal from the error signal
 *
 * \param network the network to feed
 * \param number_of_output the length of the output signal
 * \param desired the desired output signal
 *
 * \return the error value
 */
BrainDouble backpropagate(BrainNetwork network, const BrainUint number_of_output, const BrainSignal desired);
/**
 * \fn void apply_network_correction(BrainNetwork network)
 * \brief apply all corrections to reduce the total error
 *
 * \param network the BrainNetwork to correct
 */
void apply_network_correction(BrainNetwork network);
#endif /* BRAIN_NETWORK_H */
