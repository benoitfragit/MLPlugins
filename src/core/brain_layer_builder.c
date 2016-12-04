#include "brain_layer_builder.h"

struct Layer
{
    BrainNeuron* _neurons;
    BrainInt _number_of_neuron;
} Layer;

void
set_layer_input(BrainLayer layer, const BrainInt number_of_inputs, const BrainDouble* in)
{
    BrainInt j = 0;
    if (layer)
    {
        for (j = 0; j < layer->_number_of_neuron; ++j)
        {
            BrainNeuron input_neuron = get_layer_neuron(layer, j);
            set_neuron_input(input_neuron, number_of_inputs, in);
        }
    }
}

BrainNeuron
get_layer_neuron(BrainLayer layer, const BrainInt neuron_index)
{
    if (layer != NULL
    &&  0 <= neuron_index
    && neuron_index < layer->_number_of_neuron)
    {
        return layer->_neurons[neuron_index];
    }

    return NULL;
}

void
delete_layer(BrainLayer layer)
{
    BrainInt i;

    if (layer)
    {
        if (layer->_neurons)
        {
            for (i = 0; i < layer->_number_of_neuron; ++i)
            {
                delete_neuron(layer->_neurons[i]);
            }

            free(layer->_neurons);
        }
        free(layer);
    }
}

BrainLayer
new_layer_from_context(Context context)
{
    Context neuron_context;
    BrainLayer _layer = NULL;
    BrainInt index = 0;

    if (!context || !is_node_with_name(context, "layer"))
    {
        return NULL;
    }

    _layer                    = (BrainLayer)malloc(sizeof(Layer));
    _layer->_number_of_neuron = get_number_of_node_with_name(context, "neuron");

    if (_layer->_number_of_neuron > 0)
    {
        _layer->_neurons = (BrainNeuron *)malloc(_layer->_number_of_neuron * sizeof(BrainNeuron));

        for (index = 0; index < _layer->_number_of_neuron; ++index)
        {
            neuron_context = get_node_with_name_and_index(context, "neuron", index);

            if (neuron_context)
            {
                _layer->_neurons[index] = new_neuron_from_context(neuron_context);
            }
        }
    }

    return _layer;
}

BrainInt
get_layer_number_of_neuron(BrainLayer layer)
{
    if (layer)
    {
        return layer->_number_of_neuron;
    }

    return 0;
}

void
update_layer_weight(BrainLayer layer)
{
    BrainInt j = 0;
    BrainNeuron pNeuron = NULL;

    if (layer != NULL)
    {
        for (j = 0; j < get_layer_number_of_neuron(layer); ++j)
        {
            pNeuron = get_layer_neuron(layer, j);

            if (pNeuron)
            {
                update_neuron(pNeuron);
            }
        }
    }
}

void
dump_layer(BrainLayer layer, const BrainInt layer_idx, FILE* file)
{
    BrainInt i;
    if (layer && file)
    {
        for (i = 0; i < layer->_number_of_neuron; ++i)
        {
            dump_neuron(layer->_neurons[i], layer_idx, i, file);
        }
    }
}
