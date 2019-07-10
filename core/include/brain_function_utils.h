#ifndef BRAIN_FUNCTION_UTILS_H
#define BRAIN_FUNCTION_UTILS_H

BrainActivationFunction brain_activation_function(BrainString name);
BrainActivationFunction brain_derivative_function(BrainString name);
BrainCostFunction brain_cost_function(BrainString name);
BrainCostFunction brain_derivative_cost_function(BrainString name);

#endif /* BRAIN_FUNCTION_UTILS_H */
