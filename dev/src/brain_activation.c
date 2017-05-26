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

static BrainDouble
identity(const BrainDouble value)
{
    return value;
}

static BrainDouble
identity_derivative(const BrainDouble value)
{
    return 1.0;
}

static BrainDouble
sigmoid(const BrainDouble value)
{
    return 1.0 / (1.0 + exp(-value));
}

static BrainDouble
sigmoid_derivative(const BrainDouble value)
{
    const BrainDouble v = sigmoid(value);
    return v * (1.0 - v);
}

static BrainDouble
tangeant_hyperbolic(const BrainDouble value)
{
    return tanh(value);
}

static BrainDouble
tangeant_hyperbolic_derivative(const BrainDouble value)
{
    const BrainDouble v = tanh(value);
    return 1.0 - v*v;
}

static BrainDouble
co_tangeant(const BrainDouble value)
{
    return atan(value);
}

static BrainDouble
co_tangeant_derivative(const BrainDouble value)
{
    return 1.0 / (1.0 + value*value);
}

static BrainDouble
softplus(const BrainDouble value)
{
    return log(1.0 + exp(value));
}

static BrainDouble
softplus_derivative(const BrainDouble value)
{
    const BrainDouble v = exp(value);
    return v / (1.0 + v);
}

static BrainDouble
sinusoid(const BrainDouble value)
{
    return sin(value);
}

static BrainDouble
sinusoid_derivative(const BrainDouble value)
{
    return cos(value);
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
