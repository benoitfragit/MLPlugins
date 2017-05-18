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
 * \param weighted_loss each time a new input arrived each weight contributes to a certain rate of error
 * \return a new BrainWeight
 */
BrainWeight new_weight(const BrainDouble limit, BrainSignal weighted_loss);
/**
 * \fn void delete_weight(BrainWeight w)
 * \brief deleta a weight
 *
 * \param w the BrainWeight to delete
 */
void delete_weight(BrainWeight w);
/**
 * \fn void set_weight_delta(BrainWeight w, const BrainDouble delta)
 * \brief set the delta value
 *
 * \param w a BrainWeight
 * \param delta new delta value
 */
void set_weight_delta(BrainWeight w, const BrainDouble delta);
/**
 * \fn void set_weight_gradient(BrainWeight w, const BrainDouble grad)
 * \brief set the gradient value
 *
 * \param w a BrainWeight
 * \param grad new gradient value
 */
void set_weight_gradient(BrainWeight w, const BrainDouble grad);
/**
 * \fn void set_weight_correction(BrainWeight w, const BrainDouble corr)
 * \brief set the correction value
 *
 * \param w a BrainWeight
 * \param corr new correction value
 */
void set_weight_correction(BrainWeight w, const BrainDouble corr);
/**
 * \fn void set_weight_value(BrainWeight w, const BrainDouble val)
 * \brief set the weight value
 *
 * \param w a BrainWeight
 * \param val new weight value
 */
void set_weight_value(BrainWeight w, const BrainDouble val);
/**
 * \fn BrainDouble get_weight_delta(const BrainWeight w)
 * \brief get the delta value
 *
 * \param w a BrainWeight
 * \return delta value
 */
BrainDouble get_weight_delta(const BrainWeight w);
/**
 * \fn BrainDouble get_weight_correction(const BrainWeight w)
 * \brief get the correctin value
 *
 * \param w a BrainWeight
 * \return correction value
 */
BrainDouble get_weight_correction(const BrainWeight w);
/**
 * \fn BrainDouble get_weight_gradient(const BrainWeight w)
 * \brief get the gradient value
 *
 * \param w a BrainWeight
 * \return gradient value
 */
BrainDouble get_weight_gradient(const BrainWeight w);
/**
 * \fn BrainDouble get_weight_value(const BrainWeight w)
 * \brief get the weight value
 *
 * \param w a BrainWeight
 * \return weight value
 */
BrainDouble get_weight_value(const BrainWeight w);
/**
 * \fn void apply_weight_correction(BrainWeight w)
 * \brief apply the correction
 *
 * \param w A BrainWeight
 */
void apply_weight_correction(BrainWeight w);
/**
 * \fn void update_weight_loss(BrainWeight w, const BrainDouble loss)
 * \brief apply a new loss
 *
 * \param w A BrainWeight
 * \param loss the new loss
 */
void update_weight_loss(BrainWeight w, const BrainDouble loss);
#endif /* BRAIN_WEIGHT_H */
