/**
 * \file brain_settings.h
 * \brief Define the API to control network settings
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to access network settings
 */
#ifndef BRAIN_SETTINGS_H
#define BRAIN_SETTINGS_H

#include "brain_types.h"

/**
 * \fn BrainSettings new_settings(const BrainActivationType   activation_type,
 *                                const BrainCostFunctionType costfunction_type,
 *                                const BrainBool             use_dropout,
 *                                const BrainDouble           dropout_factor,
 *                                const BrainLearningType     learning_type,
 *                                const BrainDouble           backpropagation_learning_rate,
 *                                const BrainDouble           resilient_delta_min,
 *                                const BrainDouble           resilient_delta_max,
 *                                const BrainDouble           resilient_eta_positive,
 *                                const BrainDouble           resilient_eta_negative)
 * \brief return a BrainSettings struct owned by the Network to share with neurons
 *
 * \param activation_type               Choose how all neurons compute their activation
 * \param costfunction_type             Choose how the network compute its output error
 * \param use_dropout                   Choose to enable or disable the dropout
 * \param dropout_factor                Choose the number of deactivated neurons while training in percent
 * \param learning_type                 Choose how the network learn either Backpropagation or resilient training
 * \param backpropagation_learning_rate Choose the learning rate for backpropagation
 * \param resilient_delta_min           Choose a minimum value for delta typacally 1e-6
 * \param resilient_delta_max           Choose a maximum value for delta typically 50
 * \param resilient_eta_positive        Choose the resilient learning rate, when gradient derivative sign is not changing
 * \param resilient_eta_negative        Choose the resilient learning rate when gradient derivate sign is changing
 */
BrainSettings
new_settings(const BrainActivationType   activation_type,
             const BrainCostFunctionType costfunction_type,
             const BrainBool             use_dropout,
             const BrainDouble           dropout_factor,
             const BrainLearningType     learning_type,
             const BrainDouble           backpropagation_learning_rate,
             const BrainDouble           resilient_delta_min,
             const BrainDouble           resilient_delta_max,
             const BrainDouble           resilient_eta_positive,
             const BrainDouble           resilient_eta_negative);
/**
 * \fn void delete_settings(BrainSettings settings)
 * \brief free all associated memory
 *
 * \param settings Network associated settings
 */
void delete_settings(BrainSettings settings);
/**
 * \fn LearningPtrFunc get_settings_learning_function(const BrainSettings settings)
 * \brief get the network learning type
 *
 * \param settings Network associated settings
 * \return the learning type
 */
LearningPtrFunc get_settings_learning_function(const BrainSettings settings);
/**
 * \fn BrainDouble get_settings_resilient_delta_max(const BrainSettings settings)
 * \brief get the network resilient delta max
 *
 * \param settings Network associated settings
 * \return the delta max
 */
BrainDouble get_settings_resilient_delta_max(const BrainSettings settings);
/**
 * \fn BrainDouble get_settings_resilient_delta_min(const BrainSettings settings)
 * \brief get the network resilient delta min
 *
 * \param settings Network associated settings
 * \return the delta min
 */
BrainDouble get_settings_resilient_delta_min(const BrainSettings settings);
/**
 * \fn BrainDouble get_settings_resilient_eta_negative(const BrainSettings settings)
 * \brief get the network resilient eta negative
 *
 * \param settings Network associated settings
 * \return the eta negative
 */
BrainDouble get_settings_resilient_eta_negative(const BrainSettings settings);
/**
 * \fn BrainDouble get_settings_resilient_eta_positive(const BrainSettings settings)
 * \brief get the network resilient delta min
 *
 * \param settings Network associated settings
 * \return the eta positive
 */
BrainDouble get_settings_resilient_eta_positive(const BrainSettings settings);
/**
 * \fn BrainDouble get_settings_dropout_percent(const BrainSettings settings)
 * \brief get the network dropout percent
 *
 * \param settings Network associated settings
 * \return the dropout percent
 */
BrainDouble get_settings_dropout_percent(const BrainSettings settings);
/**
 * \fn BrainBool get_settings_dropout_activated(const BrainSettings settings)
 * \brief is the network use dropout
 *
 * \param settings Network associated settings
 * \return is dropout activated
 */
BrainBool get_settings_dropout_activated(const BrainSettings settings);
/**
 * \fn PtrFunc get_settings_neuron_activation(const BrainSettings settings)
 * \brief get a function pointer to compute the neuron activation
 *
 * \param settings Network associated settings
 * \return a function pointer
 */
PtrFunc get_settings_neuron_activation(const BrainSettings settings);
/**
 * \fn PtrFunc get_settings_neuron_derivative(const BrainSettings settings)
 * \brief get a function pointer to compute the neuron activation derivative
 *
 * \param settings Network associated settings
 * \return a function pointer
 */
PtrFunc get_settings_neuron_derivative(const BrainSettings settings);
/**
 * \fn PtrFunc get_settings_network_cost_function(const BrainSettings settings)
 * \brief get a function pointer to compute the network error
 *
 * \param settings Network associated settings
 * \return a function pointer
 */
CostPtrFunc get_settings_network_cost_function(const BrainSettings settings);
/**
 * \fn PtrFunc get_settings_network_cost_function_derivative(const BrainSettings settings)
 * \brief get a function pointer to compute the network error derivative
 *
 * \param settings Network associated settings
 * \return a function pointer
 */
CostPtrFunc get_settings_network_cost_function_derivative(const BrainSettings settings);
/**
 * \fn BrainDouble get_settings_backpropagation_learning_rate(const BrainSettings settings)
 * \brief get the backpropagation learning rate
 *
 * \param settings Network associated settings
 * \return a learning rate
 */
BrainDouble get_settings_backpropagation_learning_rate(const BrainSettings settings);
#endif /* BRAIN_SETTINGS_H */
