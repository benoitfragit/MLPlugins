#include "brain_neuron_builder.h"
#include "brain_activation.h"
#include "brain_settings.h"

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

    BrainDouble   _bias;            /*!< Bias of the neuron                                 */
    BrainWeight   _w;               /*!< An array of weight without the bias                */

    BrainDouble*  _out;             /*!< An output value pointer owned by the BrainLayer    */
    BrainWeight   _weighted_deltas; /*!< An array of weighted delta owned by the BrainLayer */

    BrainDouble  _gradient_bias;    /*!< rprop bias gradient value                          */
    BrainWeight  _gradient_w;       /*!< rprop weights gradient values                      */
    BrainDouble  _delta_bias;       /*!< rprop bias delta value                             */
    BrainWeight  _delta_w;          /*!< rprop weights delta values                         */
    BrainDouble  _correction_bias;  /*!< rprop bias correction value                        */
    BrainWeight  _correction_w;     /*!< rprop weights correction value                     */
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

                neuron->_bias -= learning_rate * neuron_gradient;

                for (i = 0; i < number_of_inputs; ++i)
                {
                    const BrainDouble neuron_gradient_w = neuron_gradient * neuron->_in[i];

                    neuron->_weighted_deltas[i] += loss * neuron->_w[i];
                    neuron->_w[i]               -= (learning_rate * neuron_gradient_w);
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
                   BrainDouble *weight,
                   BrainDouble *gradient,
                   BrainDouble *delta,
                   BrainDouble *correction)
{
    if ((weight     != NULL)
    &&  (gradient   != NULL)
    &&  (delta      != NULL)
    &&  (correction != NULL))
    {
        if (0.0 < *gradient * new_weight_gradient)
        {
            *delta      = MIN(*delta * rprop_eta_positive, rprop_delta_max);
            *correction = *delta;

            if (0.0 < new_weight_gradient)
            {
                *correction *= -1.0;
            }

            *weight = *weight + *correction;
        }
        else if (*gradient * new_weight_gradient < 0.0)
        {
            *delta    = MAX(*delta * rprop_eta_negative, rprop_delta_min);
            *weight   = *weight - *correction;
            *gradient = 0.0;
        }
        else if (*gradient * new_weight_gradient == 0.0)
        {
            *correction = *delta;

            if (0.0 < new_weight_gradient)
            {
                *correction *= -1.0;
            }

            *weight = *weight + *correction;
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
                                   &(neuron->_bias),
                                   &(neuron->_gradient_bias),
                                   &(neuron->_delta_bias),
                                   &(neuron->_correction_bias));

                //then update all other weights using same technique
                for (i = 0; i < number_of_inputs; ++i)
                {
                    const BrainDouble neuron_gradient_w = neuron_gradient * neuron->_in[i];

                    //this is also valid in RProp mode
                    neuron->_weighted_deltas[i] += loss * neuron->_w[i];

                    apply_neuron_rprop(rprop_eta_positive,
                                       rprop_eta_negative,
                                       rprop_delta_max,
                                       rprop_delta_min,
                                       neuron_gradient_w,
                                       &(neuron->_w[i]),
                                       &(neuron->_gradient_w[i]),
                                       &(neuron->_delta_w[i]),
                                       &(neuron->_correction_w[i]));
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
        neuron->_w[index] = weight;
    }
}

void
set_neuron_bias(BrainNeuron       neuron,
                const BrainDouble bias)
{
    if (neuron != NULL)
    {
        neuron->_bias = bias;
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
                sum += neuron->_in[j] * neuron->_w[j];
            }

            sum += neuron->_bias;

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
            free(neuron->_w);
        }

        if (neuron->_delta_w)
        {
            free(neuron->_delta_w);
        }

        if (neuron->_gradient_w)
        {
            free(neuron->_gradient_w);
        }

        if (neuron->_correction_w)
        {
            free(neuron->_correction_w);
        }

        free(neuron);
    }
}

BrainNeuron
new_neuron(const BrainUint           number_of_inputs,
           BrainSettings             settings,
           BrainDouble*              out,
           BrainWeight               weighted_delta)
{
    if ((out              != NULL)
    &&  (weighted_delta   != NULL)
    &&  (number_of_inputs != 0))
    {
        BrainUint                index = 0;
        BrainDouble random_value_limit = 0.0;

        BrainNeuron _neuron       = (BrainNeuron)calloc(1, sizeof(Neuron));
        _neuron->_out             = out;
        _neuron->_number_of_input = number_of_inputs;
        _neuron->_w               = (BrainWeight)calloc(_neuron->_number_of_input, sizeof(BrainDouble));
        _neuron->_correction_w    = (BrainWeight)calloc(_neuron->_number_of_input, sizeof(BrainDouble));
        _neuron->_gradient_w      = (BrainWeight)calloc(_neuron->_number_of_input, sizeof(BrainDouble));
        _neuron->_delta_w         = (BrainWeight)calloc(_neuron->_number_of_input, sizeof(BrainDouble));
        _neuron->_weighted_deltas = weighted_delta;
        _neuron->_settings        = settings;

        _neuron->_delta_bias      = 0.01;
        _neuron->_correction_bias = 0.0;
        _neuron->_gradient_bias   = 0.0;

        //initialize all weights with random value
        //and initialize all rprop delta
        random_value_limit = 1.0/(BrainDouble)(_neuron->_number_of_input);
        _neuron->_bias     =  (BrainDouble)rand() / (BrainDouble)RAND_MAX * 2.0 * random_value_limit - random_value_limit;

        for (index = 0; index < _neuron->_number_of_input; ++index)
        {
            _neuron->_w[index]            = (BrainDouble)rand() / (BrainDouble)RAND_MAX * 2.0 * random_value_limit - random_value_limit;
            _neuron->_delta_w[index]      = 0.01;
            _neuron->_gradient_w[index]   = 0.0;
            _neuron->_correction_w[index] = 0.0;
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
        fprintf(file, "bias=\"%lf\">\n",        neuron->_bias);

        for (i = 0; i < neuron->_number_of_input; ++i)
        {
            fprintf(file, "\t\t<weight value=\"%lf\"/>\n", neuron->_w[i]);
        }

        fprintf(file, "\t</neuron>\n");
    }
}
