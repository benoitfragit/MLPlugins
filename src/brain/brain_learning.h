#ifndef BRAIN_LEARNING_H
#define BRAIN_LEARNING_H

#include "brain_types.h"

/**
 * \fn BrainLearningType get_learning_type(BrainString learning_name)
 * \brief get the learning type
 *
 * \param learning_name the Learning name
 * \return a BrainLearningType
 */
BrainLearningType get_learning_type(BrainString learning_name);
/**
 * \fn LearningPtrFunc get_learning_function(const BrainLearningType learning_type)
 * \brief get the learning function
 *
 * \param learning_type the learning type
 * \return a function pointer to a learning function
 */
LearningPtrFunc get_learning_function(const BrainLearningType learning_type);
#endif /* BRAIN_LEARNING */
