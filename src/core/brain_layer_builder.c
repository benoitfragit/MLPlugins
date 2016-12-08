#include "brain_layer_builder.h"

struct Layer
{
    BrainNeuron* _neurons;
    BrainSignal  _out;
    BrainInt     _number_of_neuron;
} Layer;

void
set_layer_input(BrainLayer layer, const BrainInt number_of_inputs, const BrainSignal in)
{
    if (layer)
    {
        BrainInt j = 0;

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
    if (layer)
    {
        if (layer->_neurons)
        {
            BrainInt i;

            for (i = 0; i < layer->_number_of_neuron; ++i)
            {
                delete_neuron(layer->_neurons[i]);
            }

            free(layer->_neurons);
        }

        if (layer->_out)
        {
            free(layer->_out);
        }

        free(layer);
    }
}

BrainLayer
new_layer_from_context(Context context)
{
    if (context && is_node_with_name(context, "layer"))
    {
        BrainLayer _layer = NULL;

        _layer                    = (BrainLayer)calloc(1, sizeof(Layer));
        _layer->_number_of_neuron = get_number_of_node_with_name(context, "neuron");

        if (_layer->_number_of_neuron > 0)
        {
            BrainInt index = 0;
            Context neuron_context;

            _layer->_neurons = (BrainNeuron *)calloc(_layer->_number_of_neuron, sizeof(BrainNeuron));
            _layer->_out     = (BrainSignal)calloc(_layer->_number_of_neuron, sizeof(BrainDouble));

            memset(_layer->_out, 0, _layer->_number_of_neuron * sizeof(BrainDouble));

            for (index = 0; index < _layer->_number_of_neuron; ++index)
            {
                neuron_context = get_node_with_name_and_index(context, "neuron", index);

                if (neuron_context)
                {
                    _layer->_neurons[index] = new_neuron_from_context(neuron_context, &(_layer->_out[index]));
                }
            }
        }

        return _layer;
    }

    return NULL;
}

BrainSignal
get_layer_output(BrainLayer layer)
{
    if (layer != NULL)
    {
        return layer->_out;
    }

    return NULL;
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
    if (layer != NULL)
    {
        BrainInt j = 0;

        for (j = 0; j < get_layer_number_of_neuron(layer); ++j)
        {
            BrainNeuron pNeuron = get_layer_neuron(layer, j);

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
    if (layer && file)
    {
        BrainInt i;

        for (i = 0; i < layer->_number_of_neuron; ++i)
        {
            dump_neuron(layer->_neurons[i], layer_idx, i, file);
        }
    }
}
