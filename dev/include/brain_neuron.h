/**
 * \file brain_neuron.h
 * \brief Define the API to create an BrainNeuron
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to create aa BrainNeuron
 */
#ifndef BRAIN_NEURON_H
#define BRAIN_NEURON_H

#include "brain_types.h"

/**
 * \fn BrainNeuron new_neuron(const BrainUint number_of_inputs,
 *                            BrainSignal out,
 *                            BrainSignal errors)
 * \brief method to build a neuron
 *
 * \param number_of_inputs input_signal_size
 * \param out            a pointer to a BrainDouble owned by the BrainLayer
 * \param errors an array owned by the BrainLayer to update weights
 * \return a BrainNeuron or NULL if it failed
 */
BrainNeuron new_neuron(const BrainUint     number_of_inputs,
                       BrainSignal         out,
                       BrainSignal         errors);
/**
 * \fn void delete_neuron(BrainNeuron neuron)
 * \brief free all BrainNeuron memory
 *
 * \param neuron a BrainNeuron
 */
void        delete_neuron              (BrainNeuron       neuron);
/**
 * \fn void activate_neuron(BrainNeuron neuron, const BrainBool   is_an_hidden_unit)
 * \brief activate the input neuron
 *
 * \param neuron            a BrainNeuron
 * \param is_an_hidden_unit is this neuron in an hidden layer
 */
void activate_neuron(BrainNeuron neuron, const BrainBool is_an_hidden_unit);
/**
 * \fn void set_neuron_bias(BrainNeuron neuron, const BrainDouble bias)
 * \brief set the bias of a neuron
 *
 * \param neuron The neuron
 * \param bias   The neuron's bias
 */
void set_neuron_bias(BrainNeuron neuron,
                     const BrainDouble bias);
/**
 * \fn void set_neuron_weight(BrainNeuron neuron, const BrainUint index, const BrainDouble weight)
 * \brief set a weight of a neuron
 *
 * \param neuron The neuron
 * \param index  The neuron's weight index
 * \param weight The weight
 */
void set_neuron_weight(BrainNeuron neuron,
                       const BrainUint index,
                       const BrainDouble weight);
/**
 * \fn BrainUint get_neuron_number_of_input(const BrainNeuron neuron)
 * \brief retrieve the number of input
 *
 * \param neuron the Neuron
 * \return the number of input
 */
BrainUint get_neuron_number_of_input(const BrainNeuron neuron);
/**
 * \fn BrainWeight get_neuron_bias(const BrainNeuron neuron)
 * \brief retrieve neuron bias
 *
 * \param neuron the Neuron
 * \return the neuron bias
 */
BrainWeight get_neuron_bias(const BrainNeuron neuron);
/**
 * \fn BrainWeight get_neuron_weight(const BrainNeuron neuron, const BrainUint index)
 * \brief retrieve a neuron weight
 *
 * \param neuron the Neuron
 * \param index the weight inde
 * \return the neuron weight
 */
BrainWeight get_neuron_weight(const BrainNeuron neuron, const BrainUint index);
/**
 * \fn void apply_neuron_correction(BrainNeuron reuron)
 * \brief apply correction to a neuron to reduce the total error
 *
 * \param neuron a BrainNeuron
 */
void apply_neuron_correction(BrainNeuron neuron);



void
set_neuron_parameters(BrainNeuron neuron,
                     const BrainActivationType   activation_type,
                     const BrainBool             use_dropout,
                     const BrainDouble           dropout_factor,
                     const BrainLearningType     learning_type,
                     const BrainDouble           backpropagation_learning_rate,
                     const BrainDouble           resilient_delta_min,
                     const BrainDouble           resilient_delta_max,
                     const BrainDouble           resilient_eta_positive,
                     const BrainDouble           resilient_eta_negative);
void neuron_learning(BrainNeuron neuron, const BrainDouble loss);
#endif /* BRAIN_NEURON_H */
