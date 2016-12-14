#include "brain_neuron_builder.h"

struct Neuron
{
    BrainSignal  _in;
    BrainWeight  _w;
    BrainWeight  _weighted_deltas;
    BrainDouble* _out;
    BrainDouble  _out_derivative;
    BrainDouble  _bias;
    BrainDouble  _learning_rate;
    PtrFunc      _activation;
    PtrFunc      _derivative;
    BrainUint    _number_of_input;
} Neuron;

BrainDouble
get_neuron_output(const BrainNeuron neuron)
{
    if (neuron != NULL)
    {
        return *(neuron->_out);
    }

    return 0.0;
}

BrainDouble
get_neuron_bias(const BrainNeuron neuron)
{
    if (neuron != NULL)
        return neuron->_bias;

    return 0.0;
}

void
set_neuron_delta(BrainNeuron neuron,
                 const BrainDouble delta)
{
    if (neuron != NULL)
    {
        const BrainUint   number_of_inputs       = neuron->_number_of_input;
        const BrainDouble neuron_delta           = neuron->_out_derivative * delta;
        const BrainDouble neuron_learning_delta  = neuron_delta * neuron->_learning_rate;
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
set_neuron_bias(BrainNeuron neuron,
                const BrainDouble bias)
{
    if (neuron != NULL)
    {
        neuron->_bias = bias;
    }
}

void
set_neuron_input(BrainNeuron neuron,
                 const BrainUint number_of_inputs,
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

BrainDouble
get_neuron_weight(const BrainNeuron neuron,
                  const BrainUint weight_index)
{
    if (neuron != NULL
    && weight_index < neuron->_number_of_input)
    {
        return neuron->_w[weight_index];
    }

    return 0.0;
}

BrainDouble
get_neuron_input(const BrainNeuron neuron,
                 const BrainUint input_index)
{
    if (neuron != NULL
    && input_index < neuron->_number_of_input)
    {
        return neuron->_in[input_index];
    }

    return 0.0;
}

void
activate_neuron(BrainNeuron neuron)
{
    if (neuron != NULL)
    {
        BrainUint     j = 0;
        BrainDouble sum = 0.0;

        for (j = 0; j < neuron->_number_of_input; ++j)
        {
            sum += neuron->_in[j] * neuron->_w[j];
        }

        sum += neuron->_bias;

        *(neuron->_out) = neuron->_activation(sum);
        neuron->_out_derivative = neuron->_derivative(*(neuron->_out));
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
new_neuron_from_context(Context context,
                        BrainDouble* out,
                        BrainWeight  weighted_delta)
{
    if (context
    &&  is_node_with_name(context, "neuron")
    && (out         != NULL)
    && (weighted_delta != NULL))
    {
        BrainUint    index = 0;
        BrainDouble random_value_limit = 0.0;
        BrainChar*  buffer = NULL;
        BrainActivationType activation_type = Invalid_Activation;

        BrainNeuron _neuron       = (BrainNeuron)calloc(1, sizeof(Neuron));

        _neuron->_learning_rate   = node_get_double(context, "learning-rate", 0.0);
        _neuron->_out             = out;
        _neuron->_number_of_input = node_get_int(context, "input", 0);
        _neuron->_w               = (BrainWeight)calloc(_neuron->_number_of_input , sizeof(BrainDouble));
        _neuron->_weighted_deltas = weighted_delta;
         buffer                   = (BrainChar *)node_get_prop(context, "activation-type");

        activation_type      = get_activation_type(buffer);
        _neuron->_activation = activation(activation_type);
        _neuron->_derivative = derivative(activation_type);
        random_value_limit   = 1.0/(BrainDouble)(_neuron->_number_of_input);

        for (index = 0; index < _neuron->_number_of_input; ++index)
        {
            _neuron->_w[index] = (BrainDouble)rand() / (BrainDouble)RAND_MAX * 2.0 * random_value_limit - random_value_limit;
        }

        _neuron->_bias =  (BrainDouble)rand() / (BrainDouble)RAND_MAX * 2.0 * random_value_limit - random_value_limit;

        free(buffer);

        return _neuron;
    }

    return NULL;
}

void
dump_neuron(const BrainNeuron neuron,
            const BrainUint layer_idx,
            const BrainUint neuron_idx,
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
            fprintf(file, "\t\t<weight value=\"%lf\"", neuron->_w[i]);
            fprintf(file, " input=\"%d\"/>\n",i);
        }

        fprintf(file, "\t</neuron>\n");
    }
}

BrainUint
get_neuron_number_of_inputs(const BrainNeuron neuron)
{
    if (neuron != NULL)
    {
        return neuron->_number_of_input;
    }

    return 0;
}

void
set_neuron_weight(BrainNeuron neuron,
                  const BrainUint index,
                  const BrainDouble weight)
{
    if ((neuron != NULL)
    &&  (index < neuron->_number_of_input))
    {
        neuron->_w[index] = weight;
    }
}

void
initialize_neuron_from_context(BrainNeuron neuron,
                               Context context)
{
    if (neuron && context)
    {
        const BrainDouble neuron_bias       = node_get_double(context, "bias", 0.0);
        const BrainUint   number_of_weights = get_number_of_node_with_name(context, "weight");
        const BrainUint   number_of_inputs  = get_neuron_number_of_inputs(neuron);
        BrainUint index = 0;

        set_neuron_bias(neuron, neuron_bias);

        for (index = 0; index < number_of_weights; ++index)
        {
            Context subcontext = get_node_with_name_and_index(context, "weight", index);

            const BrainUint   input_idx = node_get_int   (subcontext, "input",  -1);
            const BrainDouble weight    = node_get_double(subcontext, "value",  0.0);

            if (input_idx < number_of_inputs)
            {
                set_neuron_weight(neuron, input_idx, weight);
            }
        }
    }
}
