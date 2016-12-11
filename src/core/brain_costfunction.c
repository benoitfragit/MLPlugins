#include "brain_costfunction.h"
#include <math.h>
#include <string.h>

static BrainString costfunction_names[] =
{
    "quadratic",
    "cross-entropy"
};

static BrainDouble
quadratic_cost(const BrainDouble output,
               const BrainDouble desired)
{
    return 0.5 * (output - desired) * (output - desired);
}

static BrainDouble
quadratic_cost_derivative(const BrainDouble output,
                     const BrainDouble desired)
{
    return output - desired;
}

static BrainDouble
crossentropy_cost(const BrainDouble output,
                  const BrainDouble desired)
{
    return - (desired * log(output) + (1.0 - desired) * log(1.0 - output));
}

static BrainDouble
crossentropy_cost_derivative(const BrainDouble output,
                             const BrainDouble desired)
{
    return (output - desired) * (1.0 / (output * (1.0 - output)));
}

BrainCostFunctionType
get_cost_function_type(BrainString name)
{
    BrainUint i = 0;

    for (i = First_CostFunction; i != Last_CostFunction; ++i)
    {
        if ((i != First_CostFunction)
        &&  (!strcmp(name, costfunction_names[i - First_CostFunction - 1])))
        {
            return i;
        }
    }

    return Invalid_CostFunction;
}

CostPtrFunc
get_cost_function(const BrainCostFunctionType type)
{
    switch (type)
    {
        case Quadratic:    return &quadratic_cost;
        case CrossEntropy: return &crossentropy_cost;
        default:
            break;
    }

    return &quadratic_cost;
}

CostPtrFunc
get_cost_function_derivative(const BrainCostFunctionType type)
{
    switch (type)
    {
        case Quadratic:    return &quadratic_cost_derivative;
        case CrossEntropy: return &crossentropy_cost_derivative;
        default:
            break;
    }

    return &quadratic_cost_derivative;
}
