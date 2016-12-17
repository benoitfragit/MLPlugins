/**
 * \file brain_neuron_builder.h
 * \brief Define the API to create an BrainNeuron
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to create aa BrainNeuron
 */
#ifndef BRAIN_NEURON_BUILDER_H
#define BRAIN_NEURON_BUILDER_H

#include "brain_types.h"

/**
 * \fn BrainNeuron new_neuron(const BrainUint number_of_inputs, const BrainActivationType activation_type, BrainDouble* out, BrainWeight weighted_delta)
 * \brief method to build a neuron
 *
 * \param number_of_inputs input_signal_size
 * \param activation_type the activation function type
 * \param out            a pointer to a BrainDouble owned by the BrainLayer
 * \param weighted_delta an array owned by the BrainLayer to update weighted deltas
 * \return a BrainNeuron or NULL if it failed
 */
BrainNeuron new_neuron(const BrainUint           number_of_inputs,
                       const BrainActivationType activation_type,
                       BrainDouble*              out,
                       BrainWeight               weighted_delta);
/**
 * \fn void initialize_neuron_from_context(BrainNeuron neuron, Context context)
 * \brief load all previously trained weights
 *
 * \param neuron a BrainNeuron
 * \param context the Context to use
 */
void        initialize_neuron_from_context(BrainNeuron    neuron,
                                           Context        context);
/**
 * \fn void delete_neuron(BrainNeuron neuron)
 * \brief free all BrainNeuron memory
 *
 * \param neuron a BrainNeuron
 */
void        delete_neuron              (BrainNeuron       neuron);
/**
 * \fn void dump_neuron(const BrainNeuron neuron, const BrainUint layer_idx, const BrainUint neuron_idx, FILE* file)
 * \brief serialize a neuron to an XML file
 *
 * \param neuron     A BrainNeuron
 * \param layer_idx  the BrainLayer index
 * \param neuron_idx the BrainNeuron index
 * \param file       the file to serialize in
 */
void        dump_neuron                (const BrainNeuron neuron,
                                        const BrainUint   layer_idx,
                                        const BrainUint   neuron_idx,
                                        FILE*             file);
/**
 * \fn void set_neuron_input(BrainNeuron neuron, const BrainUint number_of_inputs, const BrainSignal in, const BrainBool use_dropout, const BrainDouble dropout_percent)
 * \brief set the input signal on a neuron
 *
 * Each time a new input comes to a Neuron then if the neuron stay activated because of the
 * dropout selection then we compute the activation
 *
 * \param neuron           a BrainNeuron
 * \param number_of_inputs input signal's length
 * \param in               the input signal
 * \param use_dropout      enable or disable dropout
 * \param dropout_percent  dropout factor
 */
void        set_neuron_input           (BrainNeuron       neuron,
                                        const BrainUint   number_of_inputs,
                                        const BrainSignal in,
                                        const BrainBool   use_dropout,
                                        const BrainDouble dropout_percent);
/**
 * \fn void set_neuron_delta(BrainNeuron neuron, const BrainDouble learning_rate, const BrainDouble delta);
 * \brief set the delta of the neuron
 *
 * We first update the neuron's bias
 * Then we update the layer weighted deltas
 * Finally we update neuron's weight
 *
 * \param neuron a BrainNeuron
 * \param learning_rate the learning rate
 * \param delta  the delta
 */
void        set_neuron_delta           (BrainNeuron       neuron,
                                        const BrainDouble learning_rate,
                                        const BrainDouble delta);
#endif /* BRAIN_NEURON_BUILDER_H */
