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
 * \fn BrainNetwork new_network_from_context(Context context)
 * \brief Fonction to create a BrainNetwork from an XML context
 *
 * \param context an XML context
 * \return a new allocated BrainNetwork or NULL if it failed
 */
BrainNetwork new_network_from_context(Context context);
/**
 * \fn void delete_network(BrainNetwork network)
 * \brief deallocate a BrainNetwork
 *
 * \param network BrainNetwork to deallocate
 */
void delete_network(BrainNetwork network);
/**
 * \fn void initialize_network_from_context(BrainNetwork network, Context context)
 * \brief load previously trained neural network's weight
 *
 * \param network BrainNetwork to be initialized
 * \param context Context to load all weight
 */
void initialize_network_from_context(BrainNetwork network,
                                     Context context);
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
 * \fn BrainUint get_network_number_of_layer(const BrainNetwork network)
 * \brief get the number of layer of the network
 *
 * \param network the BrainNetwork
 * \return the number of layer
 */
BrainUint   get_network_number_of_layer(const BrainNetwork network);
/**
 * \fn BrainSignal get_network_output(const BrainNetwork network)
 * \brief get the output of the network
 *
 * \param network the BrainNetwork
 * \return get the output signal
 */
BrainSignal get_network_output(const BrainNetwork network);
/**
 * \fn BrainLayer  get_network_layer(const BrainNetwork network, const BrainUint layer_index)
 * \brief get a layer from a network
 *
 * \param network the BrainNetwork
 * \param layer_index index of the layer
 * \return a BrainLayer or NULL
 */
BrainLayer  get_network_layer(const BrainNetwork network,
                              const BrainUint layer_index);
/**
 * \fn BrainResult train(BrainNetwork network, const BrainData data, const BrainDouble target_error, const BrainUint max_iter)
 * \brief launch the training process for the neural network
 *
 * \param network      the BrainNetwork to train
 * \param data         Training dataset
 * \param target_error the minimum error threshold
 * \param max_iter     maximum number of iteration
 *
 * \return BRAIN_SUCCESS if training work else BRAIN_FAILED
 */
BrainResult train(BrainNetwork      network,
                  const BrainData   data,
                  const BrainDouble target_error,
                  const BrainUint   max_iter);

#endif /* BRAIN_NETWORK_BUILDER_H */
