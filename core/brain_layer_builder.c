#include "brain_layer_builder.h"

void
set_layer_input(Layer* layer, const int number_of_inputs, const double* in)
{
    int j = 0;
    if (layer)
    {
        for (j = 0; j < layer->_number_of_neuron; ++j)
        {
            Neuron* input_neuron = neuron(layer, j);
            set_neuron_input(input_neuron, number_of_inputs, in);
        }
    }
}

Neuron*
neuron(Layer* layer, const int neuron_index)
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
delete_layer(Layer* layer)
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

Layer*
new_layer_from_context(Context context)
{
    Context neuron_context;
    Layer* _layer = NULL;
    int index = 0;

    if (!context || !is_node_with_name(context, "layer"))
    {
        fprintf (stderr, "<%s:%d> Context is not valid !\n",  __FILE__, __LINE__);
        return NULL;
    }

    _layer                    = (Layer *)malloc(sizeof(Layer));
    _layer->_id               = node_get_int(context, "id", 0);
    _layer->_number_of_neuron = get_number_of_node_with_name(context, "neuron");

    if (_layer->_number_of_neuron > 0)
    {
        _layer->_neurons = (Neuron **)malloc(_layer->_number_of_neuron * sizeof(Neuron *));

        for (index = 0; index < _layer->_number_of_neuron; ++index)
        {
            neuron_context = get_node_with_name_and_index(context, "neuron", index);

            if (neuron_context)
            {
                _layer->_neurons[index] = new_neuron_from_context(neuron_context);

                xmlFree(neuron_context);
            }
        }
    }

    return _layer;
}
