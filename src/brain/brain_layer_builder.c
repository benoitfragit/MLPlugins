#include "brain_layer_builder.h"
#include "brain_neuron_builder.h"

/**
 * \struct Layer
 * \brief  Internal model for a BrainLayer
 *
 * All protected fields for a BrainLayer
 */
struct Layer
{
    BrainNeuron* _neurons;          /*!< An array of BrainNeuron       */
    BrainSignal  _out;              /*!< Output vector of the Layer    */
    BrainWeight  _weighted_deltas;  /*!< An array of weighted delta    */
    BrainUint    _number_of_neuron; /*!< The number of BrainNeuron     */
    BrainLayer   _next_layer;       /*!< The next layer in the network */
} Layer;

void
set_layer_input(BrainLayer        layer,
                const BrainUint   number_of_inputs,
                const BrainSignal in)
{
    if (layer != NULL)
    {
        const BrainUint number_of_neurons = layer->_number_of_neuron;
        BrainUint j = 0;

        for (j = 0; j < number_of_neurons; ++j)
        {
            BrainNeuron input_neuron = get_layer_neuron(layer, j);

            set_neuron_input(input_neuron,
                             number_of_inputs,
                             in);
        }

        if (layer->_next_layer != NULL)
        {
            set_layer_input(layer->_next_layer,
                            number_of_neurons,
                            layer->_out);
        }
    }
}

BrainNeuron
get_layer_neuron(const BrainLayer layer,
                 const BrainUint index)
{
    if ((layer != NULL)
    &&  (index < layer->_number_of_neuron))
    {
        return layer->_neurons[index];
    }

    return NULL;
}

void
delete_layer(BrainLayer layer)
{
    if (layer != NULL)
    {
        if (layer->_neurons)
        {
            BrainUint i;

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

        if (layer->_weighted_deltas)
        {
            free(layer->_weighted_deltas);
        }

        free(layer);
    }
}

void
set_layer_next_layer(BrainLayer layer,
                     BrainLayer next_layer)
{
    if (layer != NULL)
    {
        layer->_next_layer = next_layer;
    }
}

BrainLayer
get_layer_next_layer(const BrainLayer layer)
{
    if (layer != NULL)
        return layer->_next_layer;

    return NULL;
}

BrainLayer
new_layer(const BrainUint     number_of_neurons,
          const BrainUint     number_of_inputs,
          const BrainSettings settings)
{
    if ((number_of_inputs  != 0)
    &&  (number_of_neurons != 0))
    {
        BrainLayer _layer = NULL;

        _layer                    = (BrainLayer)calloc(1, sizeof(Layer));
        _layer->_next_layer       = NULL;
        _layer->_number_of_neuron = number_of_neurons;

        if (0 != _layer->_number_of_neuron)
        {
            BrainUint index = 0;

            _layer->_neurons         = (BrainNeuron *)calloc(_layer->_number_of_neuron, sizeof(BrainNeuron));
            _layer->_out             = (BrainSignal  )calloc(_layer->_number_of_neuron, sizeof(BrainDouble));
            _layer->_weighted_deltas = (BrainWeight  )calloc(_layer->_number_of_neuron, sizeof(BrainDouble));

            for (index = 0; index < _layer->_number_of_neuron; ++index)
            {
                _layer->_neurons[index] = new_neuron(number_of_inputs,
                                                     settings,
                                                     &(_layer->_out[index]),
                                                     _layer->_weighted_deltas);
            }
        }

        return _layer;
    }

    return NULL;
}

BrainSignal
get_layer_output(const BrainLayer layer)
{
    if (layer != NULL)
    {
        return layer->_out;
    }

    return NULL;
}

BrainUint
get_layer_number_of_neuron(const BrainLayer layer)
{
    if (layer)
    {
        return layer->_number_of_neuron;
    }

    return 0;
}

void
dump_layer(const BrainLayer layer,
           const BrainUint layer_idx,
           FILE* file)
{
    if ((layer != NULL) && file)
    {
        const BrainUint number_of_neurons = layer->_number_of_neuron;
        BrainUint i;

        for (i = 0; i < number_of_neurons; ++i)
        {
            dump_neuron(layer->_neurons[i], layer_idx, i, file);
        }
    }
}

void
reset_layer_delta(BrainLayer layer)
{
    if ((layer != NULL)
    &&  (layer->_weighted_deltas != NULL))
    {
        memset(layer->_weighted_deltas, 0, layer->_number_of_neuron * sizeof(BrainDouble));
    }
}

BrainDouble
get_layer_weighted_delta(const BrainLayer layer,
                         const BrainUint input_index)
{
    if ((layer != NULL)
    &&  (layer->_weighted_deltas != NULL)
    &&  (input_index < layer->_number_of_neuron))
    {
        return layer->_weighted_deltas[input_index];
    }

    return 0.0;
}
