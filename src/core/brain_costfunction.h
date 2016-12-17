/** \internal
 * \file brain_costfunction.h
 * \brief Define the API to retrieve cost functions
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to get an cost function
 */
#ifndef BRAIN_COSTFUNCTION_H
#define BRAIN_COSTFUNCTION_H

#include "brain_types.h"

/**
 * \fn CostPtrFunc get_cost_function(const BrainCostFunctionType type)
 * \brief get a cost function
 *
 * \param type BrainCostFunctionType to get the corresponding Cost function
 * \return a cost function or NULL
 */
CostPtrFunc get_cost_function(const BrainCostFunctionType type);
/**
 * \fn CostPtrFunc get_cost_function_derivative(const BrainCostFunctionType type)
 * \brief get a cost function derivative function
 *
 * \param type BrainCostFunctionType to get the corresponding cost function derivative function
 * \return a cost function derivative function or NULL
 */
CostPtrFunc get_cost_function_derivative(const BrainCostFunctionType type);
/**
 * \fn BrainCostFunctionType get_cost_function_type(BrainString name)
 * \brief get a BrainCostFunctionType according to its raw name
 *
 * \param name Raw name of the cost function type
 * \return a BrainCostFunctionType
 */
BrainCostFunctionType get_cost_function_type(BrainString name);

#endif /* BRAIN_COSTFUNCTION_H */
