#include "brain_synapse_builder.h"

struct Synapse
{
    int      _input_layer_index;
    int      _input_neuron_index;
    int      _output_neuron_index;
    int      _output_layer_index;
    int      _input_index;

    Neuron_t _input_neuron;
    Neuron_t _output_neuron;
} Synapse;

Synapse_t
new_synapse()
{
    Synapse_t _synapse = NULL;

    _synapse = (Synapse_t)malloc(sizeof(Synapse));

    return _synapse;
}

void
delete_synapse(Synapse_t synapse)
{
    if (synapse)
    {
        free(synapse);
    }
}

void
set_synapse_input_neuron(Synapse_t synapse, Neuron_t neuron)
{
    if (synapse)
        synapse->_input_neuron = neuron;
}

void
set_synapse_output_neuron(Synapse_t synapse, Neuron_t neuron)
{
    if (synapse)
        synapse->_output_neuron = neuron;
}

void
set_synapse_input_index(Synapse_t synapse, const int input_index)
{
    if (synapse)
        synapse->_input_index = input_index;
}

Neuron_t
get_synapse_input_neuron(Synapse_t synapse)
{
    if (synapse)
    {
        return synapse->_input_neuron;
    }

    return NULL;
}

Neuron_t
get_synapse_output_neuron(Synapse_t synapse)
{
    if (synapse)
    {
        return synapse->_output_neuron;
    }

    return NULL;
}

int
get_synapse_input_index(Synapse_t synapse)
{
    if (synapse)
    {
        return synapse->_input_index;
    }

    return -1;
}

int
get_synapse_input_layer_index(const Synapse_t synapse)
{
    if (synapse)
        return synapse->_input_layer_index;

    return -1;
}

int
get_synapse_output_layer_index(const Synapse_t synapse)
{
    if (synapse)
        return synapse->_output_layer_index;

    return -1;
}

int
get_synapse_input_neuron_index (const Synapse_t synapse)
{
    if (synapse)
        return synapse->_input_neuron_index;

    return -1;
}

int
get_synapse_output_neuron_index(const Synapse_t synapse)
{
    if (synapse)
        return synapse->_output_neuron_index;

    return -1;
}

void
set_synapse_input_layer_index  (Synapse_t synapse, const int input_layer_index)
{
    if (synapse)
        synapse->_input_layer_index = input_layer_index;
}

void
set_synapse_output_layer_index (Synapse_t synapse, const int output_layer_index)
{
    if (synapse)
        synapse->_output_layer_index = output_layer_index;
}

void
set_synapse_input_neuron_index (Synapse_t synapse, const int input_neuron_index)
{
    if (synapse)
        synapse->_input_neuron_index = input_neuron_index;
}

void
set_synapse_output_neuron_index(Synapse_t synapse, const int output_neuron_index)
{
    if (synapse)
        synapse->_output_neuron_index = output_neuron_index;
}

int
is_synapse_valid(Synapse_t synapse)
{
    if ((synapse != NULL) && (get_synapse_input_neuron(synapse) != NULL) && (get_synapse_output_neuron(synapse) != NULL))
    {
        return 1;
    }

    return 0;
}

void
activate_synapse(Synapse_t synapse)
{
    Neuron_t  input_neuron  = NULL;
    Neuron_t  output_neuron = NULL;
    int input_index = -1;

    if (is_synapse_valid(synapse))
    {
        input_neuron  = get_synapse_input_neuron(synapse);
        output_neuron = get_synapse_output_neuron(synapse);

        input_index   = get_synapse_input_index(synapse);

        propagate_neuron(output_neuron, get_neuron_output(input_neuron), input_index);
        activate_neuron(output_neuron);
    }
}

void
backpropagate_synapse(Synapse_t synapse)
{
    if (is_synapse_valid(synapse))
    {
        const Neuron_t output       = get_synapse_output_neuron(synapse);
        const int    input_index    = get_synapse_input_index(synapse);
        const double weighted_delta = get_neuron_weighted_delta(output, input_index);

        Neuron_t input              = get_synapse_input_neuron(synapse);

        append_neuron_delta(input, weighted_delta);
    }
}
