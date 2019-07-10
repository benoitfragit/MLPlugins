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
 * \fn void delete_network(BrainNetwork network)
 * \brief deallocate a BrainNetwork
 *
 * \param network BrainNetwork to deallocate
 */
void delete_network(BrainNetwork network);
/**
 * \fn void predict(BrainNetwork network, const BrainUint number_of_input, const BrainSignal in)
 * \brief propagate an input signal from the input signal to the output layer
 *
 * \param network the network to feed
 * \param number_of_input the length of the input signal
 * \param in the input signal
 */
void predict(BrainNetwork      network,
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
 * \fn void backpropagate(BrainNetwork network, const BrainUint number_of_output, const BrainSignal desired)
 * \brief compute one backpropagation
 *
 * \param network a Network
 * \param number_of_output the number of output
 * \param desired the desired output
 */
void backpropagate(BrainNetwork network, const BrainUint number_of_output, const BrainSignal desired);
/**
 * \fn void update_network(BrainNetwork network)
 * \brief apply correction to a neuron to reduce the total error
 *
 * \param network a BrainNetwork
 */
void update_network(BrainNetwork network);
/**
 * \fn void get_network_training_progress(BrainNetwork network)
 * \brief Get training progress
 * \param network the network
 * \return Current iteration level
 */
BrainReal get_network_training_progress(const BrainNetwork network);
/**
 * \fn BrainBool is_network_training_required(const BrainNetwork network)
 * \brief check if the network should be trained
 * \param network the Network
 * \return true is this network should be trained
 */
BrainBool is_network_training_required(const BrainNetwork network);
/**
 * \fn BrainReal train_network_iteration(BrainNetwork network, const BrainData data)
 * \brief launch a training step for the neural network using data
 *
 * \param network  the BrainNetwork to train
 * \param data data to train the network
 * \return the error for this training step
 */
BrainReal train_network_iteration(BrainNetwork network, const BrainData data);
/**
 * \fn void train_network_from_file(BrainNetwork network, BrainString datapath)
 * \brief launch the training process for the neural network
 *
 * \param network  the BrainNetwork to train
 * \param datapath     the datapath to train the network
 */
void train_network_from_file(BrainNetwork network, BrainString datapath);
/**
 * \fn void configure_network_with_context(BrainNetwork network, BrBrainString filepath)
 * \brief Load settings from an XML context
 *
 * \param network  the BrainNetwork to configure
 * \param filepath the XML context to use
 */
void configure_network_with_context(BrainNetwork network, BrainString filepath);
/**
 * \fn void deserialize_network(BrainNetwork network, BrainString filepath)
 * \brief load previously trained neural network's weight
 *
 * \param network BrainNetwork to be initialized
 * \param filepath XML file to load
 */
void deserialize_network(BrainNetwork network, BrainString filepath);
                 /**
 * \fn void serialize_network(const BrainNetwork network, const BrainString filepath)
 * \brief serialize a network to a file
 *
 * \param network the BrainNetwork to serialize
 * \param filepath the file that will be created
 */
void serialize_network(const BrainNetwork network, const BrainString filepath);
/**
 * \fn BrainNetwork new_network_from_context(BrainString filepath)
 * \brief Fonction to create a BrainNetwork from an XML context
 *
 * \param filepath XML Network definition file
 * \return a new allocated BrainNetwork or NULL if it failed
 */
BrainNetwork new_network_from_context(BrainString filepath);
#endif /* BRAIN_NETWORK_H */
