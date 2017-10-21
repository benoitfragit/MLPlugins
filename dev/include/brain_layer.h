/**
 * \file brain_layer.h
 * \brief Define the API to build a Layer
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to build a Layer
 */
#ifndef BRAIN_LAYER_H
#define BRAIN_LAYER_H

#include "brain_types.h"

/**
 * \fn BrainLayer new_layer(const BrainUint number_of_neurons,
 *                          const BrainUint number_of_inputs,
 *                          const BrainSignal in,
 *                          BrainSignal previous_errors)
 * \brief Fonction to create a BrainLayer from an XML context
 *
 * \param number_of_neurons Number of neurons in this layer
 * \param number_of_inputs size of the input signal
 * \param in input signal
 * \param previous_errors errors vector of the prevous layer
 *
 * \return a new allocated BrainLayer or NULL if it failed
 */
BrainLayer  new_layer                 (const BrainUint   number_of_neurons,
                                       const BrainUint   number_of_inputs,
                                       const BrainSignal in,
                                       BrainSignal       previous_errors);
/**
 * \fn BrainNeuron get_layer_neuron(const BrainLayer layer, const BrainUint index)
 * \brief get a Neuron from the layer
 *
 * \param layer a BrainLayer
 * \param index the neuron to extract
 * \return a BrainNeuron or NULL if it failed
 */
BrainNeuron get_layer_neuron          (const BrainLayer layer,
                                       const BrainUint index);
/**
 * \fn BrainUint get_layer_number_of_neuron(const BrainLayer layer)
 * \brief get the number of neuron in this layer
 *
 * \param layer a BrainLayer
 * \return the number of neuron in this layer
 */
BrainUint   get_layer_number_of_neuron(const BrainLayer layer);
/**
 * \fn BrainSignal get_layer_output(const BrainLayer layer)
 * \brief get the output of a layer
 *
 * \param layer a BrainLayer
 * \return a BrainSignal output of the this layer
 */
BrainSignal get_layer_output          (const BrainLayer layer);
/**
 * \fn void delete_layer(BrainLayer layer)
 * \brief free all layer allocated memory
 *
 * \param layer a BrainLayer
 */
void        delete_layer              (BrainLayer layer);
/**
 * \fn void backpropagate_output_layer(BrainLayer output_layer, const BrainUint number_of_output, const BrainSignal output, const BrainSignal desired)
 * \brief apply backpropagation algorithm on an output layer
 *
 * \param output_layer the BrainLayer
 * \param number_of_output the size of the output vector
 * \param desired the desired output
 */
void backpropagate_output_layer(BrainLayer output_layer,
                                const BrainUint number_of_output,
                                const BrainSignal desired);
/**
 * \fn void backpropagate_hidden_layer(BrainLayer hidden_layer)
 * \brief apply backpropagation algorithm on an hidden layer
 *
 * \param hidden_layer the BrainLayer
 */
void backpropagate_hidden_layer(BrainLayer hidden_layer);
/**
 * \fn BrainSignal get_layer_errors(BrainLayer layer)
 * \brief get the layer errors
 *
 * \param layer a BrainLayer
 * \return errors signal
 */
BrainSignal get_layer_errors(const BrainLayer layer);
/**
 * \fn void activate_layer(BrainLayer layer, const BrainBool hidden_layer)
 * \brief activate the layer
 *
 * \param layer a BrainLayer
 * \param hidden_layer is this layer an hidden layer
 */
void activate_layer(const BrainLayer layer, const BrainBool hidden_layer);
/**
 * \fn void set_layer_parameters( BrainLayer layer,
                                     const BrainActivationType   activation_type,
                                     const BrainCostFunctionType costfunction_type,
                                     const BrainBool             use_dropout,
                                     const BrainDouble           dropout_factor,
                                     const BrainLearningType     learning_type,
                                     const BrainDouble           backpropagation_learning_rate,
                                     const BrainDouble           resilient_delta_min,
                                     const BrainDouble           resilient_delta_max,
                                     const BrainDouble           resilient_eta_positive,
                                     const BrainDouble           resilient_eta_negative);
 * \brief customize neural network parameters
 *
 * \param layer             The BrainLayer to train
 * \param activation_type   The activation function type
 * \param costfunction_tye  The cost function type
 * \param use_droput        Activate or not the dropout rejection
 * \param dropout_factor    Set the dropout rejection threshold
 * \param learning_type     Set the learning function type
 * \param backpropagation_learning_rate Set the BACKPROP learning rate
 * \param resilient_delta_min Set RPROP min delta value
 * \param resilient_delta_max Set RPROP max delta value
 * \param resilient_eta_positive Set RPROP eta for positive gradient sign
 * \param resilient_eta_negative Set RPROP eta for negative gradient sign
 */
void set_layer_parameters(BrainLayer layer,
                             const BrainActivationType   activation_type,
                             const BrainCostFunctionType costfunction_type,
                             const BrainBool             use_dropout,
                             const BrainDouble           dropout_factor,
                             const BrainLearningType     learning_type,
                             const BrainDouble           backpropagation_learning_rate,
                             const BrainDouble           resilient_delta_min,
                             const BrainDouble           resilient_delta_max,
                             const BrainDouble           resilient_eta_positive,
                             const BrainDouble           resilient_eta_negative);
#endif /* BRAIN_LAYER_H */
