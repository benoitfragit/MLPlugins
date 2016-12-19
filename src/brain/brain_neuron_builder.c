#include "brain_neuron_builder.h"
#include "brain_activation.h"

/**
 * \struct Neuron
 * \brief  Internal model for a BrainNeuron
 *
 * All protected fields for a BrainNeuron
 */
struct Neuron
{
    BrainSignal  _in;               /*!< Input signal of an BrainNeuron                     */
    BrainDouble  _bias;             /*!< Bias of the neuron                                 */
    BrainWeight  _w;                /*!< An array of weight without the bias                */
    BrainWeight  _weighted_deltas;  /*!< An array of weighted delta owned by the BrainLayer */
    BrainDouble* _out;              /*!< An output value pointer owned by the BrainLayer    */
    BrainDouble  _out_derivative;   /*!< Derivation of the output                           */
    PtrFunc      _activation;       /*!< Activation functon of the neuron                   */
    PtrFunc      _derivative;       /*!< Activation derivative function of the neuron       */
    BrainUint    _number_of_input;  /*!< Number of inputs of the neuron                     */
} Neuron;

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
activate_neuron(BrainNeuron       neuron,
                const BrainDouble dropout_factor)
{
    if (neuron != NULL)
    {
        *(neuron->_out)         = 0.0;
        neuron->_out_derivative = 0.0;

        if (dropout_factor != 0.0)
        {
            BrainUint     j = 0;
            BrainDouble sum = 0.0;

            for (j = 0; j < neuron->_number_of_input; ++j)
            {
                sum += neuron->_in[j] * neuron->_w[j];
            }

            sum += neuron->_bias;

            *(neuron->_out) = neuron->_activation(sum) * dropout_factor;
            neuron->_out_derivative = neuron->_derivative(*(neuron->_out));
        }
    }
}

void
set_neuron_delta(BrainNeuron       neuron,
                 const BrainDouble learning_rate,
                 const BrainDouble delta)
{
    if (neuron != NULL)
    {
        const BrainUint   number_of_inputs       = neuron->_number_of_input;
        const BrainDouble neuron_delta           = neuron->_out_derivative * delta;
        const BrainDouble neuron_learning_delta  = neuron_delta * learning_rate;
        BrainUint i = 0;

        neuron->_bias -= neuron_learning_delta;

        for (i = 0; i < number_of_inputs; ++i)
        {
            neuron->_weighted_deltas[i] += neuron_delta * neuron->_w[i];
            neuron->_w[i]               -= (neuron_learning_delta * neuron->_in[i]);
        }
    }
}

void
set_neuron_input(BrainNeuron       neuron,
                 const BrainUint   number_of_inputs,
                 const BrainSignal in,
                 const BrainBool   use_dropout,
                 const BrainDouble dropout_percent)
{
    if ((in     != NULL)
    &&  (neuron != NULL)
    &&  (neuron->_number_of_input == number_of_inputs))
    {
        neuron->_in = in;
        BrainDouble dropout_factor = 1.0;

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

        activate_neuron(neuron, dropout_factor);
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
           const BrainActivationType activation_type,
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
        _neuron->_activation      = activation(activation_type);
        _neuron->_derivative      = derivative(activation_type);
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
