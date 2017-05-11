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
 * \fn void new_settings(const BrainUint             iterations,
 *                                const BrainDouble           error,
 *                                const BrainActivationType   activation_type,
 *                                const BrainCostFunctionType costfunction_type,
 *                                const BrainBool             use_dropout,
 *                                const BrainDouble           dropout_factor,
 *                                const BrainLearningType     learning_type,
 *                                const BrainDouble           backpropagation_learning_rate,
 *                                const BrainDouble           resilient_delta_min,
 *                                const BrainDouble           resilient_delta_max,
 *                                const BrainDouble           resilient_eta_positive,
 *                                const BrainDouble           resilient_eta_negative)
 * \brief initialize the settings manager
 *
 * \param iterations                    Choose training maximum number of iteration
 * \param error                         Choose training target error
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
void
new_settings(const BrainUint             iterations,
             const BrainDouble           error,
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
/**
 * \fn void delete_settings()
 * \brief free all associated memory
 */
void delete_settings();
/**
 * \fn LearningPtrFunc get_settings_learning_function()
 * \brief get the network learning type
 *
 * \return the learning type
 */
LearningPtrFunc get_settings_learning_function();
/**
 * \fn BrainDouble get_settings_resilient_delta_max()
 * \brief get the network resilient delta max
 *
 * \return the delta max
 */
BrainDouble get_settings_resilient_delta_max();
/**
 * \fn BrainDouble get_settings_resilient_delta_min()
 * \brief get the network resilient delta min
 *
 * \return the delta min
 */
BrainDouble get_settings_resilient_delta_min();
/**
 * \fn BrainDouble get_settings_resilient_eta_negative()
 * \brief get the network resilient eta negative
 *
 * \return the eta negative
 */
BrainDouble get_settings_resilient_eta_negative();
/**
 * \fn BrainDouble get_settings_resilient_eta_positive()
 * \brief get the network resilient delta min
 *
 * \return the eta positive
 */
BrainDouble get_settings_resilient_eta_positive();
/**
 * \fn BrainDouble get_settings_dropout_percent()
 * \brief get the network dropout percent
 *
 * \return the dropout percent
 */
BrainDouble get_settings_dropout_percent();
/**
 * \fn BrainBool get_settings_dropout_activated()
 * \brief is the network use dropout
 *
 * \return is dropout activated
 */
BrainBool get_settings_dropout_activated();
/**
 * \fn PtrFunc get_settings_neuron_activation()
 * \brief get a function pointer to compute the neuron activation
 *
 * \return a function pointer
 */
ActivationPtrFunc get_settings_neuron_activation();
/**
 * \fn PtrFunc get_settings_neuron_derivative()
 * \brief get a function pointer to compute the neuron activation derivative
 *
 * \return a function pointer
 */
ActivationPtrFunc get_settings_neuron_derivative();
/**
 * \fn PtrFunc get_settings_network_cost_function()
 * \brief get a function pointer to compute the network error
 *
 * \return a function pointer
 */
CostPtrFunc get_settings_network_cost_function();
/**
 * \fn PtrFunc get_settings_network_cost_function_derivative()
 * \brief get a function pointer to compute the network error derivative
 *
 * \return a function pointer
 */
CostPtrFunc get_settings_network_cost_function_derivative();
/**
 * \fn BrainDouble get_settings_backpropagation_learning_rate()
 * \brief get the backpropagation learning rate
 *
 * \return a learning rate
 */
BrainDouble get_settings_backpropagation_learning_rate();
/**
 * \fn BrainUint get_settings_max_iterations()
 * \brief get training max iterations
 *
 * \return number of iterations
 */
BrainUint get_settings_max_iterations();
/**
 * \fn BrainUint get_settings_target_error()
 * \brief get training target error
 *
 * \return the target error
 */
BrainDouble get_settings_target_error();
#endif /* BRAIN_SETTINGS_H */
