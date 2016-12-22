/**
 * \file brain_network_builder.h
 * \brief Define the API to create an control a Neural network
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to create and control a NeuralNetwork
 */

#ifndef BRAIN_NETWORK_BUILDER_H
#define BRAIN_NETWORK_BUILDER_H

#include "brain_types.h"

/**
 * \fn BrainNetwork new_network(const BrainUint             signal_input_length,
 *                              const BrainUint             number_of_layers,
 *                              const BrainSettings         settings,
 *                              const BrainUint             *neuron_per_layers)
 *
 * \brief Fonction to create a BrainNetwork
 *
 * \param signal_input_length Size of the input signal
 * \param number_of_layers    Number of layer in this network
 * \param settings            Network settings
 * \param neuron_per_layers   Array containing neumber of neurons per layer
 * \return a new allocated BrainNetwork or NULL if it failed
 */
BrainNetwork new_network(const BrainUint             signal_input_length,
                         const BrainUint             number_of_layers,
                         const BrainSettings         settings,
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
 * \fn void dump_network(const BrainNetwork network, BrainString filename)
 * \brief serialize the network to an output file
 *
 * \param network BrainNetwork to dump
 * \param filename the name of the output file
 */
void dump_network(const BrainNetwork network,
                  BrainString filename);
/**
 * \fn BrainSignal get_network_output(const BrainNetwork network)
 * \brief get the output of the network
 *
 * \param network the BrainNetwork
 * \return get the output signal
 */
BrainSignal get_network_output(const BrainNetwork network);
/**
 * \fn BrainLayer get_network_layer(const BrainNetwork network
 *                                  const BrainUint layer_index)
 * \brief get a layer of the network
 *
 * \param network the BrainNetwork
 * \param layer_index the layer index
 * \return get the requested layer or NULL
 */
BrainLayer get_network_layer(const BrainNetwork network,
                             const BrainUint layer_index);
/**
 * \fn BrainBool train(BrainNetwork network,
 *                     const BrainData data)
 * \brief launch the training process for the neural network
 *
 * \param network      the BrainNetwork to train
 * \param data         Training dataset
 *
 * \return BRAIN_TRUE if training work else BRAIN_FALSE
 */
BrainBool train(BrainNetwork      network,
                  const BrainData   data);

#endif /* BRAIN_NETWORK_BUILDER_H */
