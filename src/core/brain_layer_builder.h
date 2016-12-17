/**
 * \file brain_layer_builder.h
 * \brief Define the API to build a Layer
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to build a Layer
 */
#ifndef BRAIN_LAYER_BUILDER_H
#define BRAIN_LAYER_BUILDER_H

#include "brain_neuron_builder.h"
#include "brain_xml_utils.h"

/**
 * \fn BrainLayer new_layer_from_context(Context context)
 * \brief Fonction to create a BrainLayer from an XML context
 *
 * \param context an XML context
 * \return a new allocated BrainLayer or NULL if it failed
 */
BrainLayer  new_layer_from_context    (Context context);
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
 * \return a BrainLayer or NULL if it is the layer
 */
BrainLayer  get_layer_next_layer      (const BrainLayer layer);
/**
 * \fn void delete_layer(BrainLayer layer)
 * \brief free all layer allocated memory
 *
 * \param layer a BrainLayer
 */
void        delete_layer              (BrainLayer layer);
/**
 * \fn void set_layer_input(const BrainLayer layer, const BrainUint index)
 * \brief set the layer input
 *
 * this method set the input of a layer, then compute the activation
 * before sending the layer's output to the next_layer
 *
 * \param layer            a BrainLayer
 * \param number_of_inputs length of the input signal
 * \param in               the input_signal
 * \param use_dropout      enable or disable dropout
 * \param dropout_percent  dropout ratio
 */
void        set_layer_input           (BrainLayer layer,
                                       const BrainUint number_of_inputs,
                                       const BrainSignal in,
                                       const BrainBool use_dropbout,
                                       const BrainDouble dropout_percent);
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
 * \fn void reset_layer_delta(BrainLayer layer)
 * \brief reset the weigthed_delta vector
 *
 * \param layer a BrainLayer
 */
void        reset_layer_delta         (BrainLayer layer);
/**
 * \fn void get_layer_next_layer(const BrainLayer layer)
 * \brief get the next layer for a layer
 *
 * \param layer a BrainLayer
 */
void        set_layer_next_layer      (BrainLayer layer,
                                       BrainLayer next_layer);
#endif /* BRAIN_LAYER_BUILDER_H */
