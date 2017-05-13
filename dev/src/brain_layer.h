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
 *                          BrainLayer prev)
 * \brief Fonction to create a BrainLayer from an XML context
 *
 * \param number_of_neurons Number of neurons in this layer
 * \param number_of_inputs size of the input signal
 * \param prev the previous layer
 *
 * \return a new allocated BrainLayer or NULL if it failed
 */
BrainLayer  new_layer                 (const BrainUint number_of_neurons,
                                       const BrainUint number_of_inputs,
                                       BrainLayer prev);
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
 * \fn BrainDouble get_layer_weighted_delta(const BrainLayer layer, const BrainUint input_index)
 * \brief get the weighted delta for an input
 *
 * \param layer a BrainLayer
 * \param input_index index of the weighted delta to extract
 * \return the weighted_delta
 */
BrainDouble get_layer_weighted_delta  (const BrainLayer layer,
                                       const BrainUint input_index);
/**
 * \fn BrainLayer get_layer_next_layer(const BrainLayer layer)
 * \brief get the next layer
 *
 * \param layer a BrainLayerex
 * \return a BrainLayer or NULL if it is the output layer
 */
BrainLayer  get_layer_next_layer      (const BrainLayer layer);
/**
 * \fn BrainLayer get_layer_previous_layer(const BrainLayer layer)
 * \brief get the previous layer
 *
 * \param layer a BrainLayer
 * \return a BrainLayer or NULL if it is the input layer
 */
BrainLayer  get_layer_previous_layer      (const BrainLayer layer);
/**
 * \fn void delete_layer(BrainLayer layer)
 * \brief free all layer allocated memory
 *
 * \param layer a BrainLayer
 */
void        delete_layer              (BrainLayer layer);
/**
 * \fn void set_layer_input(const BrainLayer layer,
 *                          const BrainUint number_of_inputs,
 *                          const BrainSignal in)
 * \brief set the layer input
 *
 * this method set the input of a layer, then compute the activation
 * before sending the layer's output to the next_layer
 *
 * \param layer            a BrainLayer
 * \param number_of_inputs length of the input signal
 * \param in               the input_signal
 */
void        set_layer_input           (BrainLayer layer,
                                       const BrainUint number_of_inputs,
                                       const BrainSignal in);
/**
 * \fn void dump_layer(const BrainLayer layer)
 * \brief write the content of the layer to an xml file
 *
 * \param layer       a BrainLayer
 * \param layer_index the layer index
 * \param file        the file to use
 */
void        dump_layer                (const BrainLayer layer,
                                       const BrainUint layer_idx,
                                       FILE* file);
/**
 * \fn void set_layer_next_layer(BrainLayer layer, BrainLayer next_layer)
 * \brief set the next layer for a layer
 *
 * \param layer a BrainLayer
 * \param next_layer a BrainLayer
 */
void        set_layer_next_layer      (BrainLayer layer,
                                       BrainLayer next_layer);
/**
 * \fn void set_layer_previous_layer(BrainLayer layer, BrainLayer prev_layer)
 * \brief set the previous layer for a layer
 *
 * \param layer a BrainLayer
 * \param prev_layer a BrainLayer
 */
void        set_layer_previous_layer  (BrainLayer layer,
                                       BrainLayer prev_layer);
/**
 * \fn BrainDouble backpropagate_output_layer(BrainLayer output_layer, const BrainUint number_of_output, const BrainSignal desired)
 * \brief apply backpropagation algorithm on an output layer and return the error between computed and desired outputs
 *
 * \param output_layer the BrainLayer
 * \param number_of_output the size of the output vector
 * \param desired the desired output
 *
 * \return the error rate
 */
BrainDouble backpropagate_output_layer(BrainLayer output_layer,
                                       const BrainUint number_of_output,
                                       const BrainSignal desired);
/**
 * \fn void backpropagate_hidden_layer(BrainLayer hidden_layer)
 * \brief apply backpropagation algorithm on an hidden layer
 *
 * \param hidden_layer the BrainLayer
 */
void backpropagate_hidden_layer(BrainLayer hidden_layer);
#endif /* BRAIN_LAYER_H */
