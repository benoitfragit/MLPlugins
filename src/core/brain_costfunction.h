#ifndef BRAIN_COSTFUNCTION_H
#define BRAIN_COSTFUNCTION_H

#include "brain_types.h"

CostPtrFunc get_cost_function(const BrainCostFunctionType type);
CostPtrFunc get_cost_function_derivative(const BrainCostFunctionType type);
BrainCostFunctionType get_cost_function_type(BrainString name);

#endif /* BRAIN_COSTFUNCTION_H */
