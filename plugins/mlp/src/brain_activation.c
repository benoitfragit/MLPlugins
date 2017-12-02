#include "brain_activation.h"
#include <math.h>
#include <string.h>

static BrainString activation_name[] = {
    "Identity",
    "Sigmoid",
    "TanH",
    "ArcTan",
    "SoftPlus",
    "Sinus"
};

static BrainReal
identity(const BrainReal value)
{
    return value;
}

static BrainReal
identity_derivative(const BrainReal value)
{
    return (BrainReal)(1.0 + value - value);
}

static BrainReal
sigmoid(const BrainReal value)
{
    return (BrainReal)(1.0 / (1.0 + exp(-value)));
}

static BrainReal
sigmoid_derivative(const BrainReal value)
{
    const BrainReal v = sigmoid(value);
    return v * (1.0 - v);
}

static BrainReal
tangeant_hyperbolic(const BrainReal value)
{
    return (BrainReal)tanh(value);
}

static BrainReal
tangeant_hyperbolic_derivative(const BrainReal value)
{
    const BrainReal v = (BrainReal)tanh(value);
    return 1.0 - v*v;
}

static BrainReal
co_tangeant(const BrainReal value)
{
    return (BrainReal)atan(value);
}

static BrainReal
co_tangeant_derivative(const BrainReal value)
{
    return (BrainReal)(1.0 / (1.0 + value*value));
}

static BrainReal
softplus(const BrainReal value)
{
    return (BrainReal)log(1.0 + exp(value));
}

static BrainReal
softplus_derivative(const BrainReal value)
{
    const BrainReal v = exp(value);
    return v / (1.0 + v);
}

static BrainReal
sinusoid(const BrainReal value)
{
    return (BrainReal)sin(value);
}

static BrainReal
sinusoid_derivative(const BrainReal value)
{
    return (BrainReal)cos(value);
}

ActivationPtrFunc
activation(const BrainActivationType type)
{
    switch (type)
    {
        case Identity: return &identity;
        case TanH:     return &tangeant_hyperbolic;
        case ArcTan:   return &co_tangeant;
        case SoftPlus: return &softplus;
        case Sinusoid: return &sinusoid;
        case Sigmoid:
        default:
            break;
    }

    return &sigmoid;
}

ActivationPtrFunc
derivative(const BrainActivationType type)
{
    switch (type)
    {
        case Identity: return &identity_derivative;
        case TanH:     return &tangeant_hyperbolic_derivative;
        case ArcTan:   return &co_tangeant_derivative;
        case SoftPlus: return &softplus_derivative;
        case Sinusoid: return &sinusoid_derivative;
        case Sigmoid:
        default:
            break;
    }

    return &sigmoid_derivative;
}

BrainActivationType
get_activation_type(BrainString activation_type_name)
{
    if (activation_type_name)
    {
        BrainInt i = 0;

        for (i = First_Activation; i <= Last_Activation; ++i)
        {
            if ((i != Invalid_Activation)
            &&  !strcmp(activation_name[i - First_Activation - 1], activation_type_name))
            {
                return i;
            }
        }
    }

    return Invalid_Activation;
}
