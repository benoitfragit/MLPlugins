#include "brain_activation.h"
#include <math.h>
#include <string.h>

static const char* activation_name[] = {
    "identity",
    "sigmoid",
    "tanh",
    "arctan",
    "softplus",
    "sinus"
};

static BrainDouble
identity(const BrainDouble value)
{
    return value;
}

static BrainDouble
identity_derivative(const BrainDouble value)
{
    if (value != 0)
        return value / value;

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
    return value * (1.0 - value);
}

static BrainDouble
tangeant_hyperbolic(const BrainDouble value)
{
    return 2.0 / (1.0 + exp (-2.0 * value)) - 1.0;
}

static BrainDouble
tangeant_hyperbolic_derivative(const BrainDouble value)
{
    return 1.0 - value*value;
}

static BrainDouble
co_tangeant(const BrainDouble value)
{
    return atan(value);
}

static BrainDouble
co_tangeant_derivative(const BrainDouble value)
{
    return 1.0 / (1.0 + tan(value)*tan(value));
}

static BrainDouble
softplus(const BrainDouble value)
{
    return log(1 + exp(value));
}

static BrainDouble
softplus_derivative(const BrainDouble value)
{
    return (exp(value) - 1.0) / exp(value);
}

static BrainDouble
sinusoid(const BrainDouble value)
{
    return sin(value);
}

static BrainDouble
sinusoid_derivative(const BrainDouble value)
{
    return sqrt(1.0 - value*value);
}

PtrFunc
activation(const BrainActivationType type)
{
    switch (type)
    {
        case Identity:
            return &identity;
        case TanH:
            return &tangeant_hyperbolic;
        case ArcTan:
            return &co_tangeant;
        case SoftPlus:
            return &softplus;
        case Sinusoid:
            return &sinusoid;
        case Sigmoid:
        default:
            break;
    }

    return &sigmoid;
}

PtrFunc
derivative(const BrainActivationType type)
{
    switch (type)
    {
        case Identity:
            return &identity_derivative;
        case TanH:
            return &tangeant_hyperbolic_derivative;
        case ArcTan:
            return &co_tangeant_derivative;
        case SoftPlus:
            return &softplus_derivative;
        case Sinusoid:
            return &sinusoid_derivative;
        case Sigmoid:
        default:
            break;
    }

    return &sigmoid_derivative;
}

BrainActivationType
get_activation_type(BrainString activation_type_name)
{
    BrainUint i = 0;

    if (activation_type_name)
    {
        for (i = First_Activation; i <= Last_Activation; ++i)
        {
            if (i != Invalid && !strcmp(activation_name[i - First_Activation], activation_type_name))
            {
                return i;
            }
        }
    }

    return Invalid;
}
