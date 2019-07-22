/**
 * \file mlp_layer.h
 * \brief Define the API to build a Layer
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to build a Layer
 */
#ifndef MLP_LAYER_H
#define MLP_LAYER_H

#include "mlp_types.h"

/**
 * \fn MLPLayer new_layer(const BrainUint number_of_neurons,
 *                          const BrainUint number_of_inputs,
 *                          const BrainSignal in,
 *                          BrainSignal previous_errors)
 * \brief Fonction to create a MLPLayer from an XML context
 *
 * \param number_of_neurons Number of neurons in this layer
 * \param number_of_inputs size of the input signal
 * \param in input signal
 * \param previous_errors errors vector of the prevous layer
 *
 * \return a new allocated MLPLayer or NULL if it failed
 */
MLPLayer  new_layer                 (const BrainUint   number_of_neurons,
                                       const BrainUint   number_of_inputs,
                                       const BrainSignal in,
                                       BrainSignal       previous_errors);
/**
 * \fn MLPNeuron get_layer_neuron(const MLPLayer layer, const BrainUint index)
 * \brief get a Neuron from the layer
 *
 * \param layer a MLPLayer
 * \param index the neuron to extract
 * \return a MLPNeuron or NULL if it failed
 */
MLPNeuron get_layer_neuron          (const MLPLayer layer,
                                       const BrainUint index);
/**
 * \fn BrainUint get_layer_number_of_neuron(const MLPLayer layer)
 * \brief get the number of neuron in this layer
 *
 * \param layer a MLPLayer
 * \return the number of neuron in this layer
 */
BrainUint   get_layer_number_of_neuron(const MLPLayer layer);
/**
 * \fn BrainSignal get_layer_output(const MLPLayer layer)
 * \brief get the output of a layer
 *
 * \param layer a MLPLayer
 * \return a BrainSignal output of the this layer
 */
BrainSignal get_layer_output          (const MLPLayer layer);
/**
 * \fn void delete_layer(MLPLayer layer)
 * \brief free all layer allocated memory
 *
 * \param layer a MLPLayer
 */
void        delete_layer              (MLPLayer layer);
/**
 * \fn void backpropagate_output_layer(MLPLayer output_layer, const BrainUint number_of_output, const BrainSignal output, const BrainSignal desired)
 * \brief apply backpropagation algorithm on an output layer
 *
 * \param output_layer the MLPLayer
 * \param number_of_output the size of the output vector
 * \param loss the error vector
 */
void backpropagate_output_layer(MLPLayer output_layer,
                                const BrainUint number_of_output,
                                const BrainSignal loss);
/**
 * \fn void backpropagate_hidden_layer(MLPLayer hidden_layer)
 * \brief apply backpropagation algorithm on an hidden layer
 *
 * \param hidden_layer the MLPLayer
 */
void backpropagate_hidden_layer(MLPLayer hidden_layer);
/**
 * \fn BrainSignal get_layer_errors(MLPLayer layer)
 * \brief get the layer errors
 *
 * \param layer a MLPLayer
 * \return errors signal
 */
BrainSignal get_layer_errors(const MLPLayer layer);
/**
 * \fn void activate_layer(MLPLayer layer, const BrainBool hidden_layer)
 * \brief activate the layer
 *
 * \param layer a MLPLayer
 * \param hidden_layer is this layer an hidden layer
 */
void activate_layer(const MLPLayer layer, const BrainBool hidden_layer);
/**
 * \fn void set_layer_activation( MLPLayer layer, BrainString name)
 * \brief customize neural network parameters
 *
 * \param layer     The MLPLayer to train
 * \param name      The function name
 */
void set_layer_activation(MLPLayer layer, BrainString name);
/**
 * \fn void serialize_layer(MLPLayer layer, Writer writer)
 * \brief serialize a layer
 *
 * \param layer the layer to be serialized
 * \param writer the xml writer
 */
void serialize_layer(MLPLayer layer, Writer writer);
/**
 * \fn void deserialize_layer(MLPLayer layer, Context context)
 * \brief deserialize a layer
 *
 * \param layer the layer to be deserialized
 * \param context the context
 */
void deserialize_layer(MLPLayer layer, Context context);
/**
 * \fn void update_layer(MLPLayer layer,
 *                       BrainReal learning_rate,
 *                       BrainReal momentum)
 *
 * \brief apply correction to a neuron to reduce the total error
 *
 * \param layer a MLPLayer
 * \param minibatch_size size of the mini batch
 */
void update_layer(MLPLayer layer,
                  BrainReal learning_rate,
                  BrainReal momentum);
#endif /* MLP_LAYER_H */
