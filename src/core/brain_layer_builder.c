#include "brain_layer_builder.h"

struct Layer
{
    Neuron_t* _neurons;
    int _number_of_neuron;
} Layer;

void
set_layer_input(Layer_t layer, const int number_of_inputs, const double* in)
{
    int j = 0;
    if (layer)
    {
        for (j = 0; j < layer->_number_of_neuron; ++j)
        {
            Neuron_t input_neuron = get_layer_neuron(layer, j);
            set_neuron_input(input_neuron, number_of_inputs, in);
        }
    }
}

Neuron_t
get_layer_neuron(Layer_t layer, const int neuron_index)
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
delete_layer(Layer_t layer)
{
    int i;

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

Layer_t
new_layer_from_context(Context context)
{
    Context neuron_context;
    Layer_t _layer = NULL;
    int index = 0;

    if (!context || !is_node_with_name(context, "layer"))
    {
        BRAIN_CRITICAL ("Layer",  "<%s:%d> Context is not valid !\n",  __FILE__, __LINE__);
        return NULL;
    }

    _layer                    = (Layer_t)malloc(sizeof(Layer));
    _layer->_number_of_neuron = get_number_of_node_with_name(context, "neuron");

    if (_layer->_number_of_neuron > 0)
    {
        _layer->_neurons = (Neuron_t *)malloc(_layer->_number_of_neuron * sizeof(Neuron_t));

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

int
get_layer_number_of_neuron(Layer_t layer)
{
    if (layer)
    {
        return layer->_number_of_neuron;
    }

    return 0;
}

void
update_layer_weight(Layer_t layer)
{
    int j = 0;
    Neuron_t pNeuron = NULL;

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
dump_layer(Layer_t layer, const int layer_idx, FILE* file)
{
    int i;
    if (layer && file)
    {
        for (i = 0; i < layer->_number_of_neuron; ++i)
        {
            dump_neuron(layer->_neurons[i], layer_idx, i, file);
        }
    }
}
