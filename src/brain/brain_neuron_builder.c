#include "brain_neuron_builder.h"
#include "brain_activation.h"
#include "brain_settings.h"

/**
 * \struct Neuron
 * \brief  Internal model for a BrainNeuron
 *
 * All protected fields for a BrainNeuron
 */
struct Neuron
{
    BrainSignal     _in;               /*!< Input signal of an BrainNeuron                     */
    BrainDouble     _bias;             /*!< Bias of the neuron                                 */
    BrainWeight     _w;                /*!< An array of weight without the bias                */
    BrainWeight     _weighted_deltas;  /*!< An array of weighted delta owned by the BrainLayer */
    BrainDouble*    _out;              /*!< An output value pointer owned by the BrainLayer    */
    BrainDouble     _out_derivative;   /*!< Derivation of the output                           */
    BrainUint       _number_of_input;  /*!< Number of inputs of the neuron                     */
    BrainSettings   _settings;         /*!< Network settings                                   */
} Neuron;

void
update_neuron_using_backpropagation(BrainNeuron neuron,
                                    const BrainDouble loss)
{
    if (neuron != NULL)
    {
        const BrainSettings settings         = neuron->_settings;
        const BrainUint     number_of_inputs = neuron->_number_of_input;
        const BrainDouble   neuron_gradient  = neuron->_out_derivative * loss;
        BrainUint i = 0;

        if (settings != NULL)
        {
            const BrainDouble learning_rate = get_settings_backpropagation_learning_rate(settings);

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

void
update_neuron_using_resilient(BrainNeuron neuron,
                              const BrainDouble loss)
{
    if (neuron != NULL && loss != 0.0)
    {

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
activate_neuron(BrainNeuron neuron)
{
    if (neuron != NULL)
    {
        const BrainSettings settings            = neuron->_settings;
        PtrFunc             activation_function = get_settings_neuron_activation(settings);
        PtrFunc             derivative_function = get_settings_neuron_derivative(settings);
        const BrainDouble   dropout_percent     = get_settings_dropout_percent(settings);
        const BrainBool     use_dropout         = get_settings_dropout_activated(settings);
        BrainDouble         dropout_factor      = 1.0;

        *(neuron->_out)         = 0.0;
        neuron->_out_derivative = 0.0;

        if (use_dropout
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

        if (dropout_factor != 0.0)
        {
            BrainUint     j = 0;
            BrainDouble sum = 0.0;

            for (j = 0; j < neuron->_number_of_input; ++j)
            {
                sum += neuron->_in[j] * neuron->_w[j];
            }

            sum += neuron->_bias;

            *(neuron->_out)         = activation_function(sum) * dropout_factor;
            neuron->_out_derivative = derivative_function(*(neuron->_out));
        }
    }
}

void
set_neuron_input(BrainNeuron       neuron,
                 const BrainUint   number_of_inputs,
                 const BrainSignal in)
{
    if ((in     != NULL)
    &&  (neuron != NULL)
    &&  (neuron->_number_of_input == number_of_inputs))
    {
        neuron->_in = in;
        activate_neuron(neuron);
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
        _neuron->_w               = (BrainWeight)calloc(_neuron->_number_of_input , sizeof(BrainDouble));
        _neuron->_weighted_deltas = weighted_delta;
        _neuron->_settings        = settings;
        random_value_limit        = 1.0/(BrainDouble)(_neuron->_number_of_input);

        for (index = 0; index < _neuron->_number_of_input; ++index)
        {
            _neuron->_w[index] = (BrainDouble)rand() / (BrainDouble)RAND_MAX * 2.0 * random_value_limit - random_value_limit;
        }

        _neuron->_bias =  (BrainDouble)rand() / (BrainDouble)RAND_MAX * 2.0 * random_value_limit - random_value_limit;

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
