/**
 * \file mlp_network.h
 * \brief Define the API to create an control a Neural network
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to create and control a NeuralNetwork
 */

#ifndef MLP_NETWORK_H
#define MLP_NETWORK_H

#include "mlp_types.h"

/**
 * \fn void delete_network(MLPNetwork network)
 * \brief deallocate a MLPNetwork
 *
 * \param network MLPNetwork to deallocate
 */
void delete_network(MLPNetwork network);
/**
 * \fn void predict(MLPNetwork network, const BrainUint number_of_input, const BrainSignal in)
 * \brief propagate an input signal from the input signal to the output layer
 *
 * \param network the network to feed
 * \param number_of_input the length of the input signal
 * \param in the input signal
 */
void predict(MLPNetwork      network,
             const BrainUint   number_of_input,
             const BrainSignal in);
/**
 * \fn BrainSignal get_network_output(const MLPNetwork network)
 * \brief get the output of the network
 *
 * \param network the MLPNetwork
 * \return get the output signal
 */
BrainSignal get_network_output(const MLPNetwork network);
/**
 * \fn MLPLayer get_network_layer(const MLPNetwork network, const BrainUint index)
 * \brief retrieve the input layer
 *
 * \param network      the MLPNetwork to train
 * \param index         index of the requested layer
 *
 * \return a MLPLayer
 */
MLPLayer get_network_layer(const MLPNetwork network, const BrainUint index);
/**
 * \fn void backpropagate(MLPNetwork network, const BrainUint number_of_output, const BrainSignal desired)
 * \brief compute one backpropagation
 *
 * \param network a Network
 * \param number_of_output the number of output
 * \param desired the desired output
 */
void backpropagate(MLPNetwork network, const BrainUint number_of_output, const BrainSignal desired);
/**
 * \fn void update_networkMLPNetwork network, BrainReal learning_rate, BrainReal momentum)
 * \brief apply correction to a neuron to reduce the total error
 *
 * \param network a MLPNetwork
 * \param learning_rate the learning rate
 * \param momentum the momentum
 */
void update_network(MLPNetwork network, BrainReal learning_rate, BrainReal momentum);
/**
 * \fn void configure_network_with_context(MLPNetwork network, BrBrainString filepath)
 * \brief Load settings from an XML context
 *
 * \param network  the MLPNetwork to configure
 * \param filepath the XML context to use
 */
void configure_network_with_context(MLPNetwork network, BrainString filepath);
/**
 * \fn void deserialize_network(MLPNetwork network, BrainString filepath)
 * \brief load previously trained neural network's weight
 *
 * \param network MLPNetwork to be initialized
 * \param filepath XML file to load
 */
void deserialize_network(MLPNetwork network, BrainString filepath);
                 /**
 * \fn void serialize_network(const MLPNetwork network, const BrainString filepath)
 * \brief serialize a network to a file
 *
 * \param network the MLPNetwork to serialize
 * \param filepath the file that will be created
 */
void serialize_network(const MLPNetwork network, const BrainString filepath);
/**
 * \fn MLPNetwork new_network_from_context(BrainString filepath)
 * \brief Fonction to create a MLPNetwork from an XML context
 *
 * \param filepath XML Network definition file
 * \return a new allocated MLPNetwork or NULL if it failed
 */
MLPNetwork new_network_from_context(BrainString filepath);
/**
 * \fn void feedforward(LPNetwork network, const BrainUint number_of_input, const BrainSignal in, const BrainBool use_dropout)
 * \brief Propagate an input signal threw the network
 *
 * \param network
 * \param number_of_input
 * \param in
 * \param use_dropout
 */
void feedforward(MLPNetwork,
                 const BrainUint,
                 const BrainSignal,
                 const BrainBool);
#endif /* MLP_NETWORK_H */
