#include "brain_network.h"
#include "brain_costfunction.h"
#include "brain_activation.h"
#include "brain_layer.h"
#include "brain_neuron.h"
#include "brain_settings.h"
#include "brain_data_reader.h"

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

static BrainDouble
backpropagate(BrainNetwork network,
              const BrainUint number_of_output,
              const BrainSignal desired)
{
    BrainDouble error = 1.0;

    if (network != NULL && network->_output_layer != NULL)
    {
        error = backpropagate_output_layer(network->_output_layer,
                                           number_of_output,
                                           desired);

        backpropagate_hidden_layer(get_layer_previous_layer(network->_output_layer));
    }

    return error;
}

BrainLayer
get_network_layer(const BrainNetwork network, const BrainUint layer_index)
{
    if (network != NULL)
    {
        BrainUint idx = 0;
        BrainLayer layer = network->_input_layer;

        while (idx != layer_index)
        {
            ++idx;
            layer = get_layer_next_layer(layer);
        }

        return layer;
    }

    return NULL;
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
            BrainLayer layer      = NULL;
            BrainLayer next_layer = NULL;

            // create the input layer first
            _network->_input_layer = new_layer(number_of_neurons, number_of_inputs);
            number_of_inputs       = number_of_neurons;
            layer                  = _network->_input_layer;

            // then, create all layers
            for (index = 1; index < number_of_layers; ++index)
            {
                number_of_neurons = neuron_per_layers[index];
                next_layer = new_layer(number_of_neurons, number_of_inputs);
                number_of_inputs = number_of_neurons;

                set_layer_next_layer(layer, next_layer);
                set_layer_previous_layer(next_layer, layer);
                layer = next_layer;
            }

            _network->_output_layer = layer;
            _network->_output = get_layer_output(_network->_output_layer);
        }

        return _network;
    }

    return NULL;
}

void
dump_network(const BrainNetwork network,
             BrainString filename)
{

    if (network != NULL && filename != NULL)
    {
        FILE* file = fopen(filename, "w");
        if (file)
        {
            BrainLayer layer = network->_input_layer;
            BrainUint layer_index = 0;

            fprintf(file, "<init>\n");

            while (layer != NULL)
            {
                dump_layer(layer, layer_index, file);

                layer = get_layer_next_layer(layer);

                ++layer_index;
            }

            fprintf(file, "</init>\n");
            fclose(file);
        }
    }
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

BrainBool
train(BrainNetwork network,
      const BrainData data)
{
    if ((network != NULL)
    &&  (data != NULL))
    {
        const BrainSettings settings = get_settings_instance();

        if (settings != NULL)
        {
            const BrainUint   max_iter     = get_settings_max_iterations();
            const BrainDouble target_error = get_settings_target_error();
            BrainDouble error = target_error + 1.0;
            BrainUint iteration = 0;

            do
            {
                const BrainUint   index            = get_data_random_subset_index(data);
                const BrainUint   input_length     = get_data_input_length    (data);
                const BrainUint   output_length    = get_data_output_length   (data);
                const BrainUint   number_of_chunks = get_data_number_of_chunks(data, index);
                const BrainSignal output           = get_data_output          (data, index);
                BrainUint         chunk_index      = 0;

                for (chunk_index = 0; chunk_index < number_of_chunks; ++chunk_index)
                {
                    const BrainSignal input = get_data_input( data, index, chunk_index);
                    feedforward(network, input_length, input);
                    error = backpropagate(network, output_length, output);
                }

                ++iteration;
            } while ((iteration < max_iter) && (error > target_error));

            if (error < target_error)
            {
                return BRAIN_TRUE;
            }
        }
    }

    return BRAIN_FALSE;
}
