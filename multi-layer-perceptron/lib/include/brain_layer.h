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
 * \param loss the error vector
 */
void backpropagate_output_layer(BrainLayer output_layer,
                                const BrainUint number_of_output,
                                const BrainSignal loss);
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
 * \fn void set_layer_activation( BrainLayer layer, BrainString name)
 * \brief customize neural network parameters
 *
 * \param layer     The BrainLayer to train
 * \param name      The function name
 */
void set_layer_activation(BrainLayer layer, BrainString name);
/**
 * \fn void serialize_layer(BrainLayer layer, Writer writer)
 * \brief serialize a layer
 *
 * \param layer the layer to be serialized
 * \param writer the xml writer
 */
void serialize_layer(BrainLayer layer, Writer writer);
/**
 * \fn void deserialize_layer(BrainLayer layer, Context context)
 * \brief deserialize a layer
 *
 * \param layer the layer to be deserialized
 * \param context the context
 */
void deserialize_layer(BrainLayer layer, Context context);
/**
 * \fn void update_layer(BrainLayer layer,
 *                       BrainReal learning_rate,
 *                       BrainReal momentum)
 *
 * \brief apply correction to a neuron to reduce the total error
 *
 * \param layer a BrainLayer
 * \param minibatch_size size of the mini batch
 */
void update_layer(BrainLayer layer,
                  BrainReal learning_rate,
                  BrainReal momentum);
#endif /* BRAIN_LAYER_H */