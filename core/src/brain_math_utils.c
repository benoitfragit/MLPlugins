#include "brain_math_utils.h"
#include "brain_memory_utils.h"
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
/**********************************************************************/
/**                           UTILITY FUNCTIONS                      **/
/**********************************************************************/
BrainReal
dot(const BrainReal* a, const BrainReal* b, const BrainUint size)
{
    BrainReal ret = 0.;

    if (BRAIN_ALLOCATED(a) &&
        BRAIN_ALLOCATED(b) &&
        (size > 0))
    {
        BrainUint i = 0;
        for (i = 0; i < size; ++i)
        {
            ret += a[i] * b[i];
        }
    }

    return ret;
}

BrainReal
distance(const BrainReal* a, const BrainReal* b, const BrainUint size)
{
    BrainReal ret = 0.;

    if (BRAIN_ALLOCATED(a) &&
        BRAIN_ALLOCATED(b) &&
        (size > 0))
    {
        BrainUint i = 0;
        BrainReal t = 0.;
        for (i = 0; i < size; ++i)
        {
            t = a[i] - b[i];
            ret += t*t;
        }

        ret = sqrt(ret);
    }

    return ret;
}

BrainReal
norm2(const BrainReal* a, const BrainUint size)
{
    BrainReal ret = 0.;

    if (BRAIN_ALLOCATED(a) &&
        (size > 0))
    {
        BrainUint i = 0;
        for (i = 0; i < size; ++i)
        {
            ret += a[i]*a[i];
        }

        ret = sqrt(ret);
    }

    return ret;
}

void
normalization(BrainReal** signals,
             BrainReal* means,
             BrainReal* sigmas,
             const BrainUint number_of_signals,
             const BrainUint size)
{
    if (BRAIN_ALLOCATED(signals)
    &&  BRAIN_ALLOCATED(means)
    &&  BRAIN_ALLOCATED(sigmas)
    &&  (0 < size))
    {
        BrainUint i = 0;
        BrainUint j = 0;

        BRAIN_SET(means, 0, BrainReal, size);
        BRAIN_SET(sigmas, 0, BrainReal, size);
        /**************************************************************/
        /**                       GET THE MEAN                       **/
        /**************************************************************/
        for (i = 0; i < number_of_signals; ++i)
        {
            if (BRAIN_ALLOCATED(signals[i]))
            {
                for (j = 0; j < size; ++j)
                {
                    means[j] += signals[i][j] / (BrainReal) size;
                }
            }
        }
        /**************************************************************/
        /**                      GET THE VARIANCE                    **/
        /**************************************************************/
        for (i = 0; i < number_of_signals; ++i)
        {
            if (BRAIN_ALLOCATED(signals[i]))
            {
                for (j = 0; j < size; ++j)
                {
                    BrainReal diff = (signals[i][j] - means[j]);
                    sigmas[j] += ((diff * diff)/ (BrainReal)size);
                }
            }
        }
        /**************************************************************/
        /**                  CENTER AND SCALE EACH VECTOR            **/
        /**************************************************************/
        for (i = 0; i < number_of_signals; ++i)
        {
            if (BRAIN_ALLOCATED(signals[i]))
            {
                for (j = 0;j < size; ++j)
                {
                    signals[i][j] = (signals[i][j] - means[j]) / sqrt(sigmas[j]);
                }
            }
        }
    }
}
