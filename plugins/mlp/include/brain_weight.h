/**
 * \file brain_weight.h
 * \brief Define the API to create a BrainWeight
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to create aa BrainWeight
 */
#ifndef BRAIN_WEIGHT_H
#define BRAIN_WEIGHT_H

#include "brain_types.h"

/**
 * \fn BrainWeight new_weight(const BrainDouble limit, BrainSignal weighted_loss)
 * \brief build a new weight
 *
 * \param limit random value limit
 * \param error each time a new input arrived each weight contributes to a certain rate of error
 * \return a new BrainWeight
 */
BrainWeight new_weight(const BrainDouble limit, BrainSignal error);
/**
 * \fn void delete_weight(BrainWeight w)
 * \brief deleta a weight
 *
 * \param w the BrainWeight to delete
 */
void delete_weight(BrainWeight w);
/**
 * \fn void set_backprop_weight(BrainWeight w, const BrainDouble in, const BrainDouble gradient, const BrainDouble learning_rate)
 * \brief set the correction value
 *
 * \param w a BrainWeight
 * \param in the input associated to this weight
 * \param gradient the gradient value
 * \param learning_rate the learning rate value
 */
void set_backprop_weight(BrainWeight w,
                         const BrainDouble in,
                         const BrainDouble gradient,
                         const BrainDouble learning_rate);
/**
 * \fn void set_rprop_weight(BrainWeight w,
 *                           const BrainDouble in,
 *                           const BrainDouble gradient,
 *                           const BrainDouble rprop_eta_positive,
 *                           const BrainDouble rprop_eta_negative,
 *                           const BrainDouble rprop_delta_max,
 *                           const BrainDouble rprop_delta_min)
 * \brief set the correction value
 *
 * \param w a BrainWeight
 * \param in the input associated to this weight
 * \param gradient the gradient value
 * \param rprop_eta_positive the learning rate when gradient sign si positive
 * \param rprop_eta_negative the learning rate when gradient sign si negative
 * \param rprop_delta_max the delta max value
 * \param rprop_delta_min the delta min value
 */
void set_rprop_weight(BrainWeight w,
                     const BrainDouble in,
                     const BrainDouble gradient,
                     const BrainDouble rprop_eta_positive,
                     const BrainDouble rprop_eta_negative,
                     const BrainDouble rprop_delta_max,
                     const BrainDouble rprop_delta_min);
/**
 * \fn void set_weight_value(BrainWeight w, const BrainDouble val)
 * \brief set the weight value
 *
 * \param w a BrainWeight
 * \param val new weight value
 */
void set_weight_value(BrainWeight w, const BrainDouble val);
/**
 * \fn BrainDouble get_weight_value(const BrainWeight w)
 * \brief get the weight value
 *
 * \param w a BrainWeight
 * \return weight value
 */
BrainDouble get_weight_value(const BrainWeight w);
#endif /* BRAIN_WEIGHT_H */
