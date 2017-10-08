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
    BrainLayer*   _layers;           /*!< layers                         */
    BrainSignal   _output;           /*!< Output signal of the network   */
    BrainSignal   _input;            /*!< Input signal of the network    */
    BrainUint     _number_of_inputs; /*!< Number of inputs               */
    BrainUint     _number_of_layers; /*!< Number of layers               */
} Network;

BrainLayer
get_network_layer(const BrainNetwork network, const BrainUint index)
{
    BrainLayer ret = NULL;

    if ((network != NULL)
    &&  (network->_layers != NULL)
    &&  (index < network->_number_of_layers))
    {
        ret = network->_layers[index];
    }

    return ret;
}

void
backpropagate(BrainNetwork network,
              const BrainUint number_of_output,
              const BrainSignal desired)
{
    if ((network                   != NULL) &&
        (network->_layers          != NULL) &&
        (network->_number_of_layers!= 0)    &&
        (desired                   != NULL))
    {
        int i = 0;
        BrainLayer output_layer = network->_layers[network->_number_of_layers - 1];

        backpropagate_output_layer(output_layer, number_of_output, desired);

        for (i = network->_number_of_layers - 2; i >= 0; --i)
        {
            BrainLayer hidden_layer = network->_layers[i];

            backpropagate_hidden_layer(hidden_layer);
        }
    }
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
        if ((network->_layers) &&
            (network->_number_of_layers != 0))
        {
            BrainUint i = 0;
            for (i = 0; i < network->_number_of_layers; ++i)
            {
                BrainLayer layer = network->_layers[i];

                delete_layer(layer);
            }
        }

        if (network->_input)
        {
           free(network->_input);
        }

        free(network);
    }
}

BrainNetwork
new_network(const BrainUint signal_input_length,
            const BrainUint number_of_layers,
            const BrainUint *neuron_per_layers)
{
    BrainNetwork _network = NULL;

    if (neuron_per_layers != NULL)
    {
        BrainUint number_of_inputs = signal_input_length;
        _network = (BrainNetwork)calloc(1, sizeof(Network));

        _network->_number_of_inputs = signal_input_length;
        _network->_input            = (BrainSignal)calloc(signal_input_length, sizeof(BrainDouble));
        _network->_layers           = (BrainLayer *)calloc(number_of_layers, sizeof(BrainLayer));
        _network->_number_of_layers = number_of_layers;

        if (0 < number_of_layers)
        {
            BrainUint index = 0;

            for (index = 0;
                 index < number_of_layers;
               ++index)
            {
                const BrainUint number_of_neurons = neuron_per_layers[index];

                BrainSignal in = _network->_input;
                BrainSignal previous_errors = NULL;

                if (index != 0)
                {
                    in               = get_layer_output(_network->_layers[index - 1]);
                    previous_errors  = get_layer_errors(_network->_layers[index - 1]);
                    number_of_inputs = neuron_per_layers[index - 1];
                }

                _network->_layers[index] = new_layer(number_of_neurons,
                                                     number_of_inputs,
                                                     in,
                                                     previous_errors);
            }

            _network->_output = get_layer_output(_network->_layers[number_of_layers -1]);
        }
    }

    return _network;
}

void
feedforward(BrainNetwork      network,
            const BrainUint   number_of_input,
            const BrainSignal in,
            const BrainBool   use_dropout)
{
    if ((in != NULL) &&
        (network != NULL) &&
        (number_of_input == network->_number_of_inputs))
    {
        BrainUint i = 0;

        memcpy(network->_input, in, number_of_input * sizeof(BrainDouble));

        for (i = 0; i < network->_number_of_layers; ++i)
        {
            BrainLayer layer = network->_layers[i];

            activate_layer(layer, use_dropout && (i != network->_number_of_layers - 1));
        }
    }
}


void
apply_network_correction(BrainNetwork network)
{
    if ((network               != NULL) &&
        (network->_layers      != NULL) &&
        (network->_number_of_layers != 0))
    {
        BrainUint i = 0;

        for (i = 0; i < network->_number_of_layers; ++i)
        {
            BrainLayer layer = network->_layers[i];

            if (layer != NULL)
            {
                apply_layer_correction(layer);
            }
        }
    }
}
