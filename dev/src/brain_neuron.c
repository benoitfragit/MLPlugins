#include "brain_neuron.h"
#include "brain_activation.h"
#include "brain_weight.h"
#include "brain_random.h"

#define MIN(a, b) ((a) < (b) ? a : b)
#define MAX(a, b) ((a) < (b) ? b : a)

/**
 * \struct Neuron
 * \brief  Internal model for a BrainNeuron
 *
 * All protected fields for a BrainNeuron
 */
struct Neuron
{
    BrainSignal   _in;              /*!< Input signal of an BrainNeuron                     */
    BrainUint     _number_of_input; /*!< Number of inputs of the neuron                     */
    BrainWeight   _bias;            /*!< Bias of the neuron                                 */
    BrainWeight*  _w;               /*!< An array of weight without the bias                */
    BrainSignal   _out;             /*!< An output value pointer owned by the BrainLayer    */
    BrainDouble   _sum;             /*!< Summation of all input time weight                 */

    BrainDouble   _rprop_eta_plus;
    BrainDouble   _rprop_eta_minus;
    BrainDouble   _rprop_delta_min;
    BrainDouble   _rprop_delta_max;

    BrainDouble   _backprop_learning_rate;

    BrainDouble   _dropout_ratio;
    BrainBool     _use_dropout;

    ActivationPtrFunc _activation_function;
    ActivationPtrFunc _derivative_function;

    LearningPtrFunc _learning_function;
} Neuron;

static void
update_neuron_using_backpropagation(BrainNeuron neuron,
                                    const BrainDouble loss)
{
    if (neuron != NULL)
    {
        const BrainUint number_of_inputs = neuron->_number_of_input;

        const BrainDouble learning_rate = neuron->_backprop_learning_rate;
        ActivationPtrFunc derivative_function = neuron->_derivative_function;

        if (derivative_function != NULL)
        {
            const BrainDouble neuron_derivative = derivative_function(neuron->_sum);
            const BrainDouble neuron_gradient   = neuron_derivative * loss;

            BrainUint i = 0;

            set_weight_correction(neuron->_bias, - learning_rate * neuron_gradient);

            for (i = 0; i < number_of_inputs; ++i)
            {
                const BrainDouble neuron_gradient_w = neuron_gradient * neuron->_in[i];

                update_error(neuron->_w[i], loss);

                set_weight_correction(neuron->_w[i], - learning_rate * neuron_gradient_w);
            }
        }
    }
}

static void
apply_neuron_rprop(const BrainDouble rprop_eta_positive,
                   const BrainDouble rprop_eta_negative,
                   const BrainDouble rprop_delta_max,
                   const BrainDouble rprop_delta_min,
                   const BrainDouble new_weight_gradient,
                   BrainWeight weight)
{
    if (weight != NULL)
    {
        const BrainDouble gradient   = get_weight_gradient(weight);
        const BrainDouble delta      = get_weight_delta(weight);
        const BrainDouble correction = get_weight_last_correction(weight);

        BrainDouble new_delta = 0.0;
        BrainDouble new_correction = 0.0;

        if (0.0 < gradient * new_weight_gradient)
        {
            new_delta      = MIN(delta * rprop_eta_positive, rprop_delta_max);
            new_correction = new_delta;

            if (0.0 < new_weight_gradient)
            {
                new_correction *= -1.0;
            }

            set_weight_delta(weight, new_delta);
            set_weight_gradient(weight, new_weight_gradient);
            set_weight_correction(weight, new_correction);
        }
        else if (gradient * new_weight_gradient < 0.0)
        {
            new_delta = MAX(delta * rprop_eta_negative, rprop_delta_min);

            new_correction = -correction;

            set_weight_gradient(weight, 0.0);
            set_weight_correction(weight, new_correction);
            set_weight_delta(weight, new_delta);
        }
        else if (gradient * new_weight_gradient == 0.0)
        {
            new_correction = delta;

            if (0.0 < new_weight_gradient)
            {
                new_correction *= -1.0;
            }

            set_weight_gradient(weight, new_weight_gradient);
            set_weight_correction(weight, new_correction);
        }
    }
}

static void
update_neuron_using_resilient(BrainNeuron neuron,
                              const BrainDouble loss)
{
    if (neuron != NULL)
    {
        const BrainDouble rprop_eta_positive  = neuron->_rprop_eta_plus;
        const BrainDouble rprop_eta_negative  = neuron->_rprop_eta_minus;
        const BrainDouble rprop_delta_min     = neuron->_rprop_delta_min;
        const BrainDouble rprop_delta_max     = neuron->_rprop_delta_max;
        ActivationPtrFunc derivative_function = neuron->_derivative_function;

        if (derivative_function != NULL)
        {
            const BrainDouble neuron_derivative = derivative_function(neuron->_sum);
            const BrainDouble neuron_gradient   = neuron_derivative * loss;
            const BrainUint   number_of_inputs  = neuron->_number_of_input;
            BrainUint i = 0;

            //first update the bias using RProp algorithm
            apply_neuron_rprop(rprop_eta_positive,
                               rprop_eta_negative,
                               rprop_delta_max,
                               rprop_delta_min,
                               neuron_gradient,
                               neuron->_bias);

            //then update all other weights using same technique
            for (i = 0; i < number_of_inputs; ++i)
            {
                const BrainDouble neuron_gradient_w = neuron_gradient * neuron->_in[i];

                update_error(neuron->_w[i], loss);

                apply_neuron_rprop(rprop_eta_positive,
                                   rprop_eta_negative,
                                   rprop_delta_max,
                                   rprop_delta_min,
                                   neuron_gradient_w,
                                   neuron->_w[i]);
            }
        }
    }
}

static LearningPtrFunc
get_learning_function(const BrainLearningType learning_type)
{
    switch (learning_type)
    {
        case BackPropagation: return &update_neuron_using_backpropagation;
        case Resilient: return &update_neuron_using_resilient;
        default:
            break;
    }

    return &update_neuron_using_backpropagation;
}

void
set_neuron_parameters(BrainNeuron neuron,
                     const BrainActivationType   activation_type,
                     const BrainBool             use_dropout,
                     const BrainDouble           dropout_factor,
                     const BrainLearningType     learning_type,
                     const BrainDouble           backpropagation_learning_rate,
                     const BrainDouble           resilient_delta_min,
                     const BrainDouble           resilient_delta_max,
                     const BrainDouble           resilient_eta_positive,
                     const BrainDouble           resilient_eta_negative)
{
    if (neuron)
    {
        neuron->_use_dropout = use_dropout;
        neuron->_dropout_ratio = dropout_factor;

        neuron->_backprop_learning_rate = backpropagation_learning_rate;

        neuron->_rprop_delta_max = resilient_delta_max;
        neuron->_rprop_delta_min = resilient_delta_min;
        neuron->_rprop_eta_plus  = resilient_eta_positive;
        neuron->_rprop_eta_minus = resilient_eta_negative;

        neuron->_activation_function = activation(activation_type);
        neuron->_derivative_function = derivative(activation_type);

        neuron->_learning_function = get_learning_function(learning_type);
    }
}

BrainWeight
get_neuron_weight(const BrainNeuron neuron, const BrainUint index)
{
    if (neuron != NULL && index < neuron->_number_of_input)
    {
        return neuron->_w[index];
    }

    return NULL;
}

BrainWeight
get_neuron_bias(const BrainNeuron neuron)
{
    if (neuron != NULL)
    {
        return neuron->_bias;
    }

    return NULL;
}

void
set_neuron_weight(BrainNeuron       neuron,
                  const BrainUint   index,
                  const BrainDouble weight)
{
    if ((neuron != NULL)
    &&  (index < neuron->_number_of_input))
    {
        set_weight_value(neuron->_w[index], weight);
    }
}

void
set_neuron_bias(BrainNeuron       neuron,
                const BrainDouble bias)
{
    if (neuron != NULL)
    {
        set_weight_value(neuron->_bias, bias);
    }
}

void
activate_neuron(BrainNeuron neuron,
                const BrainBool is_an_hidden_unit)
{
    if (neuron != NULL)
    {
        ActivationPtrFunc activation_function = neuron->_activation_function;
        const BrainDouble dropout_percent     = neuron->_dropout_ratio;
        const BrainBool   use_dropout         = neuron->_use_dropout;
        BrainDouble       dropout_factor      = 1.0;

        *(neuron->_out)         = 0.0;
        neuron->_sum   = 0.0;

        //dropout is only available for hidden unit
        if (use_dropout
        &&  is_an_hidden_unit
        && (0 <= dropout_percent)
        && (dropout_percent < 1.0))
        {
            if (get_random_double_value() < dropout_percent)
            {
                dropout_factor = 0.0;
            }
            else
            {
                dropout_factor *= 1.0 / (1.0 - dropout_percent);
            }
        }

        if ((activation_function != NULL)
        &&  (dropout_factor != 0.0))
        {
            BrainUint     j = 0;

            neuron->_sum = 0.0;

            for (j = 0; j < neuron->_number_of_input; ++j)
            {
                neuron->_sum += neuron->_in[j] * get_weight_value(neuron->_w[j]);
            }

            neuron->_sum += get_weight_value(neuron->_bias);

            *(neuron->_out) = activation_function(neuron->_sum) * dropout_factor;
        }
    }
}

void
delete_neuron(BrainNeuron neuron)
{
    if (neuron)
    {
        if (neuron->_w != NULL)
        {
            BrainUint i;
            for (i = 0; i < neuron->_number_of_input; ++i)
            {
                if (neuron->_w[i] != NULL)
                {
                    delete_weight(neuron->_w[i]);
                }
            }

            free(neuron->_w);
        }

        if (neuron->_bias != NULL)
        {
            delete_weight(neuron->_bias);
        }

        free(neuron);
    }
}

BrainNeuron
new_neuron(const BrainUint number_of_inputs,
           BrainSignal     out,
           BrainSignal     errors)
{
    if ((out              != NULL)
    &&  (number_of_inputs != 0))
    {
        BrainUint                index = 0;
        BrainDouble random_value_limit = (BrainDouble)number_of_inputs;

        BrainNeuron _neuron       = (BrainNeuron)calloc(1, sizeof(Neuron));
        
        _neuron->_out             = out;
        _neuron->_number_of_input = number_of_inputs;
        _neuron->_w               = (BrainWeight *)calloc(_neuron->_number_of_input, sizeof(BrainWeight));
        _neuron->_bias            = new_weight(random_value_limit, NULL);
        _neuron->_sum             = 0.0;
        _neuron->_backprop_learning_rate = 1.12;
        _neuron->_rprop_eta_plus = 1.2;
        _neuron->_rprop_eta_minus = 0.95;
        _neuron->_rprop_delta_min = 0.000001;
        _neuron->_rprop_delta_max = 50;

        for (index = 0; index < _neuron->_number_of_input; ++index)
        {
            BrainSignal error = NULL;

            if (errors != NULL)
            {
                error = &(errors[index]);
            }

            _neuron->_w[index] = new_weight(random_value_limit, error);
        }

        return _neuron;
    }

    return NULL;
}

BrainUint
get_neuron_number_of_input(const BrainNeuron neuron)
{
    if (neuron != NULL)
    {
        return neuron->_number_of_input;
    }

    return 0;
}


void
apply_neuron_correction(BrainNeuron neuron)
{
    if (neuron != NULL)
    {
        const BrainUint number_of_weight = neuron->_number_of_input;
        BrainUint j = 0;
        BrainWeight w = neuron->_bias;

        if (w != NULL)
        {
            apply_weight_correction(w);
        }

        for (j = 0; j < number_of_weight; ++j)
        {
            w = neuron->_w[j];

            if (w != NULL)
            {
                apply_weight_correction(w);
            }
        }
    }
}

void
neuron_learning(BrainNeuron neuron, const BrainDouble loss)
{
    if (neuron)
    {
        LearningPtrFunc learning_function = neuron->_learning_function;

        if (learning_function)
        {
            learning_function(neuron, loss);
        }
    }
}
