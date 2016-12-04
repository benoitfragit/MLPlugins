#include "brain_synapse_builder.h"

struct Synapse
{
    BrainInt      _input_layer_index;
    BrainInt      _input_neuron_index;
    BrainInt      _output_neuron_index;
    BrainInt      _output_layer_index;
    BrainInt      _input_index;

    BrainNeuron _input_neuron;
    BrainNeuron _output_neuron;
} Synapse;

BrainSynapse
new_synapse()
{
    BrainSynapse _synapse = NULL;

    _synapse = (BrainSynapse)malloc(sizeof(Synapse));

    return _synapse;
}

void
delete_synapse(BrainSynapse synapse)
{
    if (synapse)
    {
        free(synapse);
    }
}

void
set_synapse_input_neuron(BrainSynapse synapse, BrainNeuron neuron)
{
    if (synapse)
        synapse->_input_neuron = neuron;
}

void
set_synapse_output_neuron(BrainSynapse synapse, BrainNeuron neuron)
{
    if (synapse)
        synapse->_output_neuron = neuron;
}

void
set_synapse_input_index(BrainSynapse synapse, const BrainInt input_index)
{
    if (synapse)
        synapse->_input_index = input_index;
}

BrainNeuron
get_synapse_input_neuron(BrainSynapse synapse)
{
    if (synapse)
    {
        return synapse->_input_neuron;
    }

    return NULL;
}

BrainNeuron
get_synapse_output_neuron(BrainSynapse synapse)
{
    if (synapse)
    {
        return synapse->_output_neuron;
    }

    return NULL;
}

BrainInt
get_synapse_input_index(BrainSynapse synapse)
{
    if (synapse)
    {
        return synapse->_input_index;
    }

    return -1;
}

BrainInt
get_synapse_input_layer_index(const BrainSynapse synapse)
{
    if (synapse)
        return synapse->_input_layer_index;

    return -1;
}

BrainInt
get_synapse_output_layer_index(const BrainSynapse synapse)
{
    if (synapse)
        return synapse->_output_layer_index;

    return -1;
}

BrainInt
get_synapse_input_neuron_index (const BrainSynapse synapse)
{
    if (synapse)
        return synapse->_input_neuron_index;

    return -1;
}

BrainInt
get_synapse_output_neuron_index(const BrainSynapse synapse)
{
    if (synapse)
        return synapse->_output_neuron_index;

    return -1;
}

void
set_synapse_input_layer_index  (BrainSynapse synapse, const BrainInt input_layer_index)
{
    if (synapse)
        synapse->_input_layer_index = input_layer_index;
}

void
set_synapse_output_layer_index (BrainSynapse synapse, const BrainInt output_layer_index)
{
    if (synapse)
        synapse->_output_layer_index = output_layer_index;
}

void
set_synapse_input_neuron_index (BrainSynapse synapse, const BrainInt input_neuron_index)
{
    if (synapse)
        synapse->_input_neuron_index = input_neuron_index;
}

void
set_synapse_output_neuron_index(BrainSynapse synapse, const BrainInt output_neuron_index)
{
    if (synapse)
        synapse->_output_neuron_index = output_neuron_index;
}

BrainBool
is_synapse_valid(BrainSynapse synapse)
{
    if ((synapse != NULL) && (get_synapse_input_neuron(synapse) != NULL) && (get_synapse_output_neuron(synapse) != NULL))
    {
        return BRAIN_TRUE;
    }

    return BRAIN_FALSE;
}

void
activate_synapse(BrainSynapse synapse)
{
    BrainNeuron  input_neuron  = NULL;
    BrainNeuron  output_neuron = NULL;
    BrainInt input_index = -1;

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
backpropagate_synapse(BrainSynapse synapse)
{
    if (is_synapse_valid(synapse))
    {
        const BrainNeuron output       = get_synapse_output_neuron(synapse);
        const BrainInt    input_index    = get_synapse_input_index(synapse);
        const BrainDouble weighted_delta = get_neuron_weighted_delta(output, input_index);

        BrainNeuron input              = get_synapse_input_neuron(synapse);

        append_neuron_delta(input, weighted_delta);
    }
}
