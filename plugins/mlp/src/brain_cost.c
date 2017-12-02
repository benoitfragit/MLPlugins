#include "brain_cost.h"
#include <math.h>
#include <string.h>

static BrainString costfunction_names[] =
{
    "Quadratic",
    "CrossEntropy"
};

static BrainReal
quadratic_cost(const BrainReal output,
               const BrainReal desired)
{
    return 0.5 * (output - desired) * (output - desired);
}

static BrainReal
quadratic_cost_derivative(const BrainReal output, const BrainReal desired)
{
    return output - desired;
}

static BrainReal
crossentropy_cost(const BrainReal output, const BrainReal desired)
{
    return -(desired * log(output) + (1.0 - desired) * log(1.0 - output));
}

static BrainReal
crossentropy_cost_derivative(const BrainReal output,
                             const BrainReal desired)
{
    return (output - desired) * (1.0 / (output * (1.0 - output)));
}

BrainCostFunctionType
get_cost_function_type(BrainString name)
{
    BrainUint i = 0;

    for (i = First_CostFunction; i <= Last_CostFunction; ++i)
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
