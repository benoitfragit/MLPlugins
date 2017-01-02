#include "brain_neuron_builder.h"
#include "brain_activation.h"
#include "brain_settings.h"
#include "brain_weight.h"

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
    BrainSettings _settings;        /*!< Common Network settings                                   */

    BrainSignal   _in;              /*!< Input signal of an BrainNeuron                     */
    BrainUint     _number_of_input; /*!< Number of inputs of the neuron                     */

    BrainWeight   _bias;            /*!< Bias of the neuron                                 */
    BrainWeight*  _w;               /*!< An array of weight without the bias                */

    BrainSignal _out;             /*!< An output value pointer owned by the BrainLayer    */
    BrainSignal _weighted_deltas; /*!< An array of weighted delta owned by the BrainLayer */
} Neuron;

void
update_neuron_using_backpropagation(BrainNeuron neuron,
                                    const BrainDouble loss)
{
    if (neuron != NULL)
    {
        const BrainSettings settings         = neuron->_settings;
        const BrainUint     number_of_inputs = neuron->_number_of_input;
        BrainUint i = 0;

        if (settings != NULL)
        {
            const BrainDouble learning_rate = get_settings_backpropagation_learning_rate(settings);
            PtrFunc     derivative_function = get_settings_neuron_derivative(settings);

            if (derivative_function != NULL)
            {
                const BrainDouble neuron_derivative = derivative_function(*(neuron->_out));
                const BrainDouble neuron_gradient   = neuron_derivative * loss;

                set_weight_correction(neuron->_bias, - learning_rate * neuron_gradient);
                apply_weight_correction(neuron->_bias);

                for (i = 0; i < number_of_inputs; ++i)
                {
                    const BrainDouble neuron_gradient_w = neuron_gradient * neuron->_in[i];

                    neuron->_weighted_deltas[i] += loss * get_weight_value(neuron->_w[i]);

                    set_weight_correction(neuron->_w[i], - learning_rate * neuron_gradient_w);
                    apply_weight_correction(neuron->_w[i]);
                }
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
        const BrainDouble correction = get_weight_correction(weight);

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

            apply_weight_correction(weight);
        }
        else if (gradient * new_weight_gradient < 0.0)
        {
            new_delta = MAX(delta * rprop_eta_negative, rprop_delta_min);

            new_correction = -correction;

            set_weight_gradient(weight, 0.0);
            set_weight_correction(weight, new_correction);
            set_weight_delta(weight, delta);

            apply_weight_correction(weight);
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
            apply_weight_correction(weight);
        }
    }
}

void
update_neuron_using_resilient(BrainNeuron neuron,
                              const BrainDouble loss)
{
    if (neuron != NULL)
    {
        BrainSettings settings = neuron->_settings;

        if (settings != NULL)
        {
            const BrainDouble rprop_eta_positive = get_settings_resilient_eta_positive(settings);
            const BrainDouble rprop_eta_negative = get_settings_resilient_eta_negative(settings);
            const BrainDouble rprop_delta_min    = get_settings_resilient_delta_min(settings);
            const BrainDouble rprop_delta_max    = get_settings_resilient_delta_max(settings);
            PtrFunc     derivative_function      = get_settings_neuron_derivative(settings);

            if (derivative_function != NULL)
            {
                const BrainDouble neuron_derivative = derivative_function(*(neuron->_out));
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

                    //this is also valid in RProp mode
                    neuron->_weighted_deltas[i] += loss * get_weight_value(neuron->_w[i]);

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

static void
activate_neuron(BrainNeuron neuron,
                const BrainBool is_an_hidden_unit)
{
    if (neuron != NULL)
    {
        const BrainSettings settings            = neuron->_settings;
        PtrFunc             activation_function = get_settings_neuron_activation(settings);
        const BrainDouble   dropout_percent     = get_settings_dropout_percent(settings);
        const BrainBool     use_dropout         = get_settings_dropout_activated(settings);
        BrainDouble         dropout_factor      = 1.0;

        *(neuron->_out)         = 0.0;

        //dropout is only available for hidden unit
        if (use_dropout
        &&  is_an_hidden_unit
        && (0 <= dropout_percent)
        && (dropout_percent < 1.0))
        {
            if ((BrainDouble)(rand() / RAND_MAX) < dropout_percent)
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
            BrainDouble sum = 0.0;

            for (j = 0; j < neuron->_number_of_input; ++j)
            {
                sum += neuron->_in[j] * get_weight_value(neuron->_w[j]);
            }

            sum += get_weight_value(neuron->_bias);

            *(neuron->_out) = activation_function(sum) * dropout_factor;
        }
    }
}

void
set_neuron_input(BrainNeuron       neuron,
                 const BrainUint   number_of_inputs,
                 const BrainSignal in,
                 const BrainBool   is_an_hidden_unit)
{
    if ((in     != NULL)
    &&  (neuron != NULL)
    &&  (neuron->_number_of_input == number_of_inputs))
    {
        neuron->_in = in;
        activate_neuron(neuron, is_an_hidden_unit);
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
           BrainSettings   settings,
           BrainSignal     out,
           BrainSignal     weighted_delta)
{
    if ((out              != NULL)
    &&  (weighted_delta   != NULL)
    &&  (number_of_inputs != 0))
    {
        BrainUint                index = 0;
        BrainDouble random_value_limit = (BrainDouble)number_of_inputs;

        BrainNeuron _neuron       = (BrainNeuron)calloc(1, sizeof(Neuron));
        _neuron->_out             = out;
        _neuron->_number_of_input = number_of_inputs;
        _neuron->_w               = (BrainWeight *)calloc(_neuron->_number_of_input, sizeof(BrainWeight));
        _neuron->_weighted_deltas = weighted_delta;
        _neuron->_settings        = settings;
        _neuron->_bias            = new_weight(random_value_limit);

        for (index = 0; index < _neuron->_number_of_input; ++index)
        {
            _neuron->_w[index] = new_weight(random_value_limit);
        }

        return _neuron;
    }

    return NULL;
}

void
dump_neuron(const BrainNeuron neuron,
            const BrainUint   layer_idx,
            const BrainUint   neuron_idx,
            FILE* file)
{
    if (neuron && file)
    {
        BrainUint i = 0;

        fprintf(file, "\t<neuron index=\"%d\" ", neuron_idx);
        fprintf(file, "layer-index=\"%d\" ",     layer_idx);
        fprintf(file, "bias=\"%lf\">\n",        get_weight_value(neuron->_bias));

        for (i = 0; i < neuron->_number_of_input; ++i)
        {
            fprintf(file, "\t\t<weight value=\"%lf\"/>\n", get_weight_value(neuron->_w[i]));
        }

        fprintf(file, "\t</neuron>\n");
    }
}
