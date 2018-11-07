#include "brain_math_utils.h"
#include "brain_memory_utils.h"
#include "brain_random_utils.h"
/**********************************************************************/
/**                       ACTIVATION FUNCTIONS                       **/
/**********************************************************************/
BrainReal
identity(const BrainReal value)
{
    return value;
}

BrainReal
identity_derivative(const BrainReal value)
{
    return (BrainReal)(1.0 + value - value);
}

BrainReal
sigmoid(const BrainReal value)
{
    return (BrainReal)(1.0 / (1.0 + exp(-value)));
}

BrainReal
sigmoid_derivative(const BrainReal value)
{
    const BrainReal v = sigmoid(value);
    return v * (1.0 - v);
}

BrainReal
tangeant_hyperbolic(const BrainReal value)
{
    return (BrainReal)tanh(value);
}

BrainReal
tangeant_hyperbolic_derivative(const BrainReal value)
{
    const BrainReal v = (BrainReal)tanh(value);
    return 1.0 - v*v;
}

BrainReal
co_tangeant(const BrainReal value)
{
    return (BrainReal)atan(value);
}

BrainReal
co_tangeant_derivative(const BrainReal value)
{
    return (BrainReal)(1.0 / (1.0 + value*value));
}

BrainReal
softplus(const BrainReal value)
{
    return (BrainReal)log(1.0 + exp(value));
}

BrainReal
softplus_derivative(const BrainReal value)
{
    const BrainReal v = exp(value);
    return v / (1.0 + v);
}

BrainReal
sinusoid(const BrainReal value)
{
    return (BrainReal)sin(value);
}

BrainReal
sinusoid_derivative(const BrainReal value)
{
    return (BrainReal)cos(value);
}

BrainReal
relu(const BrainReal value)
{
    return value > 0. ? value : 0.;
}

BrainReal
relu_derivative(const BrainReal value)
{
    return value > 0. ? 1. : 0.;
}

/**********************************************************************/
/**                        COST FUNCTION                             **/
/**********************************************************************/
BrainReal
quadratic_cost(const BrainReal output, const BrainReal desired)
{
    return 0.5 * (output - desired) * (output - desired);
}

BrainReal
quadratic_cost_derivative(const BrainReal output, const BrainReal desired)
{
    return output - desired;
}

BrainReal
crossentropy_cost(const BrainReal output, const BrainReal desired)
{
    return -(desired * log(output) + (1.0 - desired) * log(1.0 - output));
}

BrainReal
crossentropy_cost_derivative(const BrainReal output, const BrainReal desired)
{
    return (output - desired) * (1.0 / (output * (1.0 - output)));
}
