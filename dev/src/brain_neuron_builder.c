#include "brain_neuron_builder.h"
#include "brain_activation.h"
#include "brain_settings.h"
#include "brain_weight.h"

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
} Neuron;

BrainDouble
get_neuron_input(const BrainNeuron neuron,
                 const BrainUint index)
{
    if (neuron != NULL && index < neuron->_number_of_input)
    {
        return neuron->_in[index];
    }

    return 0.0;
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

BrainDouble
get_neuron_output(const BrainNeuron neuron)
{
    if (neuron != NULL && neuron->_out != NULL)
    {
        return *(neuron->_out);
    }

    return 0.0;
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
        PtrFunc             activation_function = get_settings_neuron_activation();
        const BrainDouble   dropout_percent     = get_settings_dropout_percent();
        const BrainBool     use_dropout         = get_settings_dropout_activated();
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
        _neuron->_bias            = new_weight(random_value_limit, NULL);

        for (index = 0; index < _neuron->_number_of_input; ++index)
        {
            _neuron->_w[index] = new_weight(random_value_limit, &(weighted_delta[index]));
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

BrainUint
get_neuron_number_of_input(const BrainNeuron neuron)
{
    if (neuron != NULL)
        return neuron->_number_of_input;

    return 0;
}
