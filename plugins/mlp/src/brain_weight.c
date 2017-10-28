#include "brain_weight.h"
#include "brain_random.h"

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
    BrainDouble _last_correction; /*!< weights last correction value  */
    BrainSignal _error;           /*!< error to correct in the layer */
} Weight;

BrainWeight
new_weight(const BrainDouble limit, BrainSignal error)
{
    BrainWeight w = (BrainWeight)calloc(1, sizeof(Weight));

    w->_value      = get_random_double_value_in_range(-limit, limit);
    w->_last_correction = 0.0;
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

void
set_weight_delta(BrainWeight w, const BrainDouble delta)
{
    if (w != NULL)
    {
        w->_delta = delta;
    }
}

void
set_weight_gradient(BrainWeight w, const BrainDouble grad)
{
    if (w != NULL)
    {
        w->_gradient = grad;
    }
}

void
set_weight_correction(BrainWeight w, const BrainDouble corr)
{
    if (w != NULL)
    {
        w->_value += corr;
        w->_last_correction = corr;
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
get_weight_delta(const BrainWeight w)
{
    if (w != NULL)
    {
        return w->_delta;
    }

    return 0.0;
}

BrainDouble
get_weight_gradient(const BrainWeight w)
{
    if (w != NULL)
    {
        return w->_gradient;
    }

    return 0.0;
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

BrainDouble
get_weight_last_correction(const BrainWeight w)
{
    if (w != NULL)
    {
        return w->_last_correction;
    }

    return 0.0;
}

void
update_error(BrainWeight w, const BrainDouble error)
{
    if (w->_error != NULL)
    {
        *(w->_error) = *(w->_error) + error * w->_value;
    }
}
