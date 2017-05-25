#include "brain_network.h"
#include "brain_cost.h"
#include "brain_activation.h"
#include "brain_layer.h"
#include "brain_neuron.h"
#include "brain_settings.h"

/**
 * \struct Network
 * \brief  Internal model for a BrainNetwork
 *
 * All protected fields for a BrainNetwork
 */
struct Network
{
    BrainLayer    _input_layer;      /*!< The input layer                */
    BrainLayer    _output_layer;     /*!< The output layer               */
    BrainSignal   _output;           /*!< Output signal of the layer     */
} Network;

BrainDouble
backpropagate(BrainNetwork network,
              const BrainUint number_of_output,
              const BrainSignal desired)
{
    BrainDouble error = 0.0;

    if ((network                   != NULL) &&
        (network->_output_layer    != NULL) &&
        (desired                   != NULL))
    {
        BrainLayer output_layer = network->_output_layer;
        BrainLayer hidden_layer = get_layer_previous_layer(output_layer);

        error = backpropagate_output_layer(output_layer,
                                           number_of_output,
                                           desired);

        while (hidden_layer != NULL)
        {
            backpropagate_hidden_layer(hidden_layer);

            hidden_layer = get_layer_previous_layer(hidden_layer);
        }
    }

    return error;
}

BrainSignal
get_network_output(const BrainNetwork network)
{
    if (network != NULL)
    {
        return network->_output;
    }

    return NULL;
}

void
delete_network(BrainNetwork network)
{
    if (network != NULL)
    {
        BrainLayer layer = network->_input_layer;

        while (layer != NULL)
        {
            BrainLayer next_layer = get_layer_next_layer(layer);

            delete_layer(layer);

            layer = next_layer;
        }

        free(network);
    }
}

BrainNetwork
new_network(const BrainUint     signal_input_length,
            const BrainUint     number_of_layers,
            const BrainUint     *neuron_per_layers)
{
    if (neuron_per_layers != NULL)
    {
        BrainUint number_of_inputs = signal_input_length;
        BrainNetwork _network      = (BrainNetwork)calloc(1, sizeof(Network));

        srand(time(NULL));

        if (0 < number_of_layers)
        {
            BrainUint index = 0;
            BrainUint number_of_neurons = neuron_per_layers[0];
            BrainLayer previous_layer = NULL;

            // create the input layer first
            _network->_input_layer = new_layer(number_of_neurons,
                                               number_of_inputs,
                                               NULL);
            number_of_inputs       = number_of_neurons;
            previous_layer         = _network->_input_layer;

            // then, create all layers
            for (index = 1; index < number_of_layers; ++index)
            {
                number_of_neurons = neuron_per_layers[index];

                _network->_output_layer = new_layer(number_of_neurons,
                                                    number_of_inputs,
                                                    previous_layer);

                number_of_inputs = number_of_neurons;

                previous_layer = _network->_output_layer;
            }

            _network->_output = get_layer_output(_network->_output_layer);
        }

        return _network;
    }

    return NULL;
}

void
feedforward(BrainNetwork      network,
            const BrainUint   number_of_input,
            const BrainSignal in)
{
    if (in != NULL && network != NULL)
    {
        set_layer_input(network->_input_layer,
                        number_of_input,
                        in);
    }
}
