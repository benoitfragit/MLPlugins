#include "brain_neuron_builder.h"

struct Neuron
{
    BrainSignal  _in;
    BrainWeight  _w;
    BrainWeight  _weighted_deltas;
    BrainDouble* _out;
    BrainDouble* _correction;
    BrainDouble  _out_derivative;
    BrainDouble  _bias;
    BrainDouble  _learning_rate;
    BrainDouble  _inertial_factor;
    BrainDouble  _delta;
    PtrFunc      _activation;
    PtrFunc      _derivative;
    BrainInt     _number_of_input;
} Neuron;

BrainDouble
get_neuron_output(BrainNeuron neuron)
{
    if (neuron)
    {
        return *(neuron->_out);
    }

    return 0.0;
}

BrainDouble get_neuron_bias(BrainNeuron neuron)
{
    if (neuron != NULL)
        return neuron->_bias;

    return 0.0;
}

void
set_neuron_delta(BrainNeuron neuron, const BrainDouble delta)
{
    if (neuron != NULL)
    {
        const BrainInt number_of_inputs = neuron->_number_of_input;
        BrainInt i = 0;

        neuron->_delta = neuron->_out_derivative * delta;

        for (i = 0; i < number_of_inputs; ++i)
        {
            neuron->_weighted_deltas[i] += neuron->_delta * neuron->_w[i];
        }
    }
}

void
set_neuron_input(BrainNeuron neuron,
                 const BrainInt number_of_inputs,
                 const BrainSignal in)
{
    if (in != NULL && neuron != NULL && neuron->_number_of_input == number_of_inputs)
    {
        neuron->_in = in;

        activate_neuron(neuron);
    }
}

BrainDouble
get_neuron_weight(BrainNeuron neuron, const BrainInt weight_index)
{
    if (neuron != NULL
    && 0 <= weight_index
    && weight_index < neuron->_number_of_input + 1)
    {
        return neuron->_w[weight_index];
    }

    return 0.0;
}

BrainDouble
get_neuron_input(BrainNeuron neuron, const BrainInt input_index)
{
    if (neuron != NULL
    && 0 <= input_index
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
        BrainInt      j = 0;
        BrainDouble sum = 0.0;

        for (j = 0; j < neuron->_number_of_input; ++j)
        {
            sum += neuron->_in[j] * neuron->_w[j];
        }

        sum += neuron->_w[neuron->_number_of_input] * neuron->_bias;

        *(neuron->_out) = neuron->_activation(sum);
        neuron->_out_derivative = neuron->_derivative(*(neuron->_out));

        //*(neuron->_out) = (*(neuron->_out) > 0) - (*(neuron->_out) < 0);

    }
}

void
update_neuron(BrainNeuron neuron)
{
    if (neuron != NULL)
    {
        BrainInt    index      = 0;
        BrainDouble correction = 0.0;

        for (index = 0; index < neuron->_number_of_input; ++index)
        {
            correction = - neuron->_learning_rate * neuron->_delta * neuron->_in[index] + neuron->_inertial_factor * neuron->_correction[index];

            neuron->_w[index] += correction;
            neuron->_correction[index] = correction;
        }

        correction = - neuron->_learning_rate * neuron->_delta * neuron->_bias + neuron->_inertial_factor * neuron->_correction[neuron->_number_of_input];
        neuron->_w[neuron->_number_of_input] += correction;
        neuron->_correction[neuron->_number_of_input] = correction;
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

        if (neuron->_correction != NULL)
        {
            free(neuron->_correction);
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
        BrainInt    index = 0;
        BrainDouble random_value_limit = 0.0;
        BrainChar*  buffer = NULL;
        BrainActivationType activation_type = Invalid;

        BrainNeuron _neuron       = (BrainNeuron)calloc(1, sizeof(Neuron));

        _neuron->_learning_rate   = node_get_double(context, "learning-rate", 0.0);
        _neuron->_inertial_factor = node_get_double(context, "inertial-factor", 0.0);
        _neuron->_out             = out;
        _neuron->_delta           = 0.0;
        _neuron->_bias            = 1.0;
        _neuron->_number_of_input = node_get_int(context, "input", 0);
        _neuron->_w               = (BrainWeight)calloc(_neuron->_number_of_input + 1, sizeof(BrainDouble));
        _neuron->_correction      = (BrainDouble *)calloc(_neuron->_number_of_input + 1, sizeof(BrainDouble)) ;
        _neuron->_weighted_deltas = weighted_delta;
         buffer                   = (BrainChar *)node_get_prop(context, "activation-type");

        activation_type      = get_activation_type(buffer);
        _neuron->_activation = activation(activation_type);
        _neuron->_derivative = derivative(activation_type);
        random_value_limit   = 1.0/(BrainDouble)(_neuron->_number_of_input + 1.0);

        for (index = 0; index < _neuron->_number_of_input + 1; ++index)
        {
            _neuron->_w[index] = (BrainDouble)rand() / (BrainDouble)RAND_MAX * 2.0 * random_value_limit - random_value_limit;
        }

        free(buffer);

        return _neuron;
    }

    return NULL;
}

void
dump_neuron(BrainNeuron neuron,
            const BrainInt layer_idx,
            const BrainInt neuron_idx,
            FILE* file)
{
    if (neuron && file)
    {
        BrainInt i = 0;
        for (i = 0; i < neuron->_number_of_input + 1; ++i)
        {
            fprintf(file, "\t<weight value=\"%lf\"", neuron->_w[i]);
            fprintf(file, " layer=\"%d\"", layer_idx);
            fprintf(file, " neuron=\"%d\"", neuron_idx);
            fprintf(file, " input=\"%d\"/>\n",i);
        }
    }
}

BrainInt
get_neuron_number_of_inputs(BrainNeuron neuron)
{
    if (neuron != NULL)
    {
        return neuron->_number_of_input + 1;
    }

    return 0;
}

void
set_neuron_weight(BrainNeuron neuron,
                  const BrainInt index,
                  const BrainDouble weight)
{
    if (neuron != NULL && 0 <= index && index < neuron->_number_of_input + 1)
    {
        neuron->_w[index] = weight;
    }
}
