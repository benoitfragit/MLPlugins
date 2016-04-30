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

static double
identity(const double value)
{
    return value;
}

static double
identity_derivative(const double value)
{
    return 1.0;
}

static double
sigmoid(const double value)
{
    return 1.0 / (1.0 + exp(-value));
}

static double
sigmoid_derivative(const double value)
{
    return value * (1.0 - value);
}

static double
tangeant_hyperbolic(const double value)
{
    return 2.0 / (1.0 + exp (-2.0 * value)) - 1.0;
}

static double
tangeant_hyperbolic_derivative(const double value)
{
    return 1.0 - value*value;
}

static double
co_tangeant(const double value)
{
    return atan(value);
}

static double
co_tangeant_derivative(const double value)
{
    return 1.0 / (1.0 + tan(value)*tan(value));
}

static double
softplus(const double value)
{
    return log(1 + exp(value));
}

static double
softplus_derivative(const double value)
{
    return (exp(value) - 1.0) / exp(value);
}

static double
sinusoid(const double value)
{
    return sin(value);
}

static double
sinusoid_derivative(const double value)
{
    return sqrt(1.0 - value*value);
}

PtrFunc
activation(const ActivationType type)
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
derivative(const ActivationType type)
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

ActivationType
get_activation_type(const char* activation_type_name)
{
    int i = 0;

    if (activation_type_name)
    {
        for (i = First_Activation; i < Last_Activation; ++i)
        {
            if (i != Invalid && !strcmp(activation_name[i - First_Activation], activation_type_name))
            {
                return i;
            }
        }
    }

    return Invalid;
}
