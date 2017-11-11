#include "brain_weight.h"
#include "brain_random.h"

#include <math.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

#define EPSILON 1e-6

/**
 * \struct Weight
 * \brief  Internal model for a BrainWeight
 *
 * All protected fields for a BrainWeight
 */
struct Weight
{
    BrainDouble _value;           /*!< Weight value                   */
    BrainDouble _gradient;        /*!< weights gradient values        */
    BrainDouble _delta;           /*!< weights delta values           */
    BrainSignal _error;           /*!< error to correct in the layer */
} Weight;

BrainWeight
new_weight(const BrainDouble limit, BrainSignal error)
{
    BrainWeight w = (BrainWeight)calloc(1, sizeof(Weight));

    w->_value      = get_random_double_value_in_range(-limit, limit);
    w->_gradient   = 0.0;
    w->_delta      = 0.01;
    w->_error      = error;

    return w;
}

void
delete_weight(BrainWeight w)
{
    if (w != NULL)
    {
        free(w);
    }
}

static void
update_error(BrainWeight w, const BrainDouble error)
{
    if (w && w->_error)
    {
        *(w->_error) = *(w->_error) + error * w->_value;
    }
}

void
set_backprop_weight(BrainWeight w,
                    const BrainDouble in,
                    const BrainDouble gradient,
                    const BrainDouble learning_rate)
{
    if (w != NULL)
    {
        // Backpropagate the error on the previous layer
        update_error(w, gradient);
        // then update the weight value
        w->_value = w->_value - learning_rate * gradient * in;
    }
}

void
set_rprop_weight(BrainWeight w,
                 const BrainDouble in,
                 const BrainDouble gradient,
                 const BrainDouble rprop_eta_positive,
                 const BrainDouble rprop_eta_negative,
                 const BrainDouble rprop_delta_max,
                 const BrainDouble rprop_delta_min)
{
    if (w != NULL)
    {
        const BrainDouble g = gradient * in;

        BrainDouble new_delta = 0.0;
        BrainDouble new_correction = 0.0;
        BrainDouble new_gradient = 0.0;

        update_error(w, gradient);

        if (0.0 < w->_gradient * g)
        {
            new_delta      = MIN(w->_delta * rprop_eta_positive, rprop_delta_max);
            new_correction = new_delta;
            new_gradient   = g;

            if (0.0 < g)
            {
                new_correction *= -1.0;
            }
        }
        else if (w->_gradient * g < 0.0)
        {
            new_delta       = MAX(w->_delta * rprop_eta_negative, rprop_delta_min);
        }
        else if (fabs(w->_gradient * g) <= EPSILON)
        {
            new_correction  = w->_delta;
            new_gradient    = g;

            if (0.0 < g)
            {
                new_correction *= -1.0;
            }
        }

        w->_value   += new_correction;
        w->_delta    = new_delta;
        w->_gradient = new_gradient;
    }
}

void
set_weight_value(BrainWeight w, const BrainDouble val)
{
    if (w != NULL)
    {
        w->_value = val;
    }
}

BrainDouble
get_weight_value(const BrainWeight w)
{
    if (w != NULL)
    {
        return w->_value;
    }

    return 0.0;
}
