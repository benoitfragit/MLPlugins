#include "brain_network_builder.h"
#include "brain_costfunction.h"
#include "brain_activation.h"
#include "brain_layer_builder.h"
#include "brain_neuron_builder.h"
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
    BrainLayer*   _layers;           /*!< An array of BrainLayer         */
    BrainSignal   _output;           /*!< Output signal of the layer     */
    BrainUint     _number_of_layer;  /*!< Number of layers               */
    BrainSettings _settings;          /*!< Network settings               */
} Network;

static BrainUint
get_network_number_of_layer(const BrainNetwork network)
{
    if (network != NULL)
    {
        return network->_number_of_layer;
    }

    return 0;
}

BrainLayer
get_network_layer(const BrainNetwork network,
                  const BrainUint layer_index)
{
    if ((network != NULL)
    &&  (network->_layers != NULL)
    &&  (layer_index < network->_number_of_layer))
    {
        return network->_layers[layer_index];
    }

    return NULL;
}


static BrainDouble
backpropagate_output_layer(BrainNetwork network,
                           const BrainUint number_of_output,
                           const BrainSignal desired)
{
    BrainDouble error = 0.0;

    if ((network != NULL)
    &&  (desired != NULL))
    {
        const BrainSettings settings = network->_settings;

        if (settings != NULL)
        {
            LearningPtrFunc   learning_function = get_settings_learning_function(settings);

            if (learning_function != NULL)
            {
                const BrainUint   number_of_layers  = get_network_number_of_layer(network);
                BrainLayer        output_layer      = get_network_layer(network, number_of_layers - 1);
                const BrainSignal output            = get_layer_output(output_layer);
                const BrainUint   number_of_neuron  = get_layer_number_of_neuron(output_layer);

                if ((output_layer     != NULL)
                &&  (number_of_neuron == number_of_output))
                {
                    CostPtrFunc cost_function            = get_settings_network_cost_function(settings);
                    CostPtrFunc cost_function_derivative = get_settings_network_cost_function_derivative(settings);

                    BrainUint output_index = 0;

                    reset_layer_delta(output_layer);

                    for (output_index = 0;
                         output_index < number_of_output;
                       ++output_index)
                    {
                        const BrainDouble loss = cost_function_derivative(output[output_index],
                                                                          desired[output_index]);

                        BrainNeuron oNeuron = get_layer_neuron(output_layer,
                                                               output_index);

                        error += cost_function(output[output_index],
                                               desired[output_index]);

                        if (oNeuron != NULL)
                        {
                            learning_function(oNeuron, loss);
                        }
                    }
                }
            }
        }
    }

    error *= 1.0/(BrainDouble)number_of_output;

    return error;
}

static void
backpropagate_hidden_layer(BrainNetwork network,
                           const BrainUint layer_index)
{
    const BrainUint number_of_layers = get_network_number_of_layer(network);

    if ((network != NULL)
    &&  (0 != layer_index)
    &&  (layer_index < number_of_layers - 1))
    {
        BrainSettings    settings = network->_settings;

        if (settings != NULL)
        {
            LearningPtrFunc  learning_function = get_settings_learning_function(settings);

            if (learning_function != NULL)
            {
                BrainLayer       current_layer            = get_network_layer(network, layer_index);
                const BrainLayer next_layer               = get_layer_next_layer(current_layer);
                const BrainInt   current_number_of_neuron = get_layer_number_of_neuron(current_layer);

                if (current_layer != NULL && next_layer != NULL)
                {
                    BrainInt i = 0;

                    reset_layer_delta(current_layer);

                    for (i = 0; i < current_number_of_neuron; ++i)
                    {
                        BrainNeuron current_neuron = get_layer_neuron(current_layer, i);

                        if (current_neuron != NULL)
                        {
                            const BrainDouble loss = get_layer_weighted_delta(next_layer, i);

                            learning_function(current_neuron, loss);
                        }
                    }
                }
            }
        }
    }
}

static BrainDouble
backpropagate(BrainNetwork network,
              const BrainUint number_of_output,
              const BrainSignal desired)
{
    BrainUint i;
    const BrainUint number_of_layers = get_network_number_of_layer(network);
    const BrainDouble error = backpropagate_output_layer(network, number_of_output, desired);

    for (i = 2; i <= number_of_layers; ++i)
    {
        backpropagate_hidden_layer(network, number_of_layers - i);
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
        if (network->_layers != NULL)
        {
            const BrainUint number_of_layers = network->_number_of_layer;
            BrainUint i = 0;

            for (i = 0; i < number_of_layers; ++i)
            {
                delete_layer(network->_layers[i]);
            }

            free(network->_layers);
        }

        if (network->_settings != NULL)
        {
            delete_settings(network->_settings);
        }

        free(network);
    }
}

BrainNetwork
new_network(const BrainUint     signal_input_length,
            const BrainUint     number_of_layers,
            const BrainSettings settings,
            const BrainUint     *neuron_per_layers)
{
    if (neuron_per_layers != NULL && settings != NULL)
    {
        BrainUint number_of_inputs = signal_input_length;
        BrainNetwork _network      = (BrainNetwork)calloc(1, sizeof(Network));

        _network->_number_of_layer = number_of_layers;
        _network->_settings        = settings;

        srand(time(NULL));

        if (_network->_number_of_layer != 0)
        {
            BrainUint                  index = 0;
            BrainLayer            last_layer = NULL;
            const BrainUint last_layer_index = _network->_number_of_layer - 1;

            _network->_layers = (BrainLayer *)calloc(_network->_number_of_layer, sizeof(BrainLayer));

            for (index = 0; index < _network->_number_of_layer; ++index)
            {
                const BrainUint number_of_neurons = neuron_per_layers[index];

                _network->_layers[index] = new_layer(number_of_neurons,
                                                     number_of_inputs,
                                                     settings);

                number_of_inputs = number_of_neurons;

                if (0 < index && _network->_layers[index] != NULL)
                {
                    set_layer_next_layer(_network->_layers[index - 1], _network->_layers[index]);
                }
            }

            last_layer = get_network_layer(_network, last_layer_index);
            _network->_output = get_layer_output(last_layer);
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
            const BrainUint number_of_layers = network->_number_of_layer;
            BrainUint i;

            fprintf(file, "<init>\n");

            for (i = 0; i < number_of_layers; ++i)
            {
                dump_layer(network->_layers[i], i, file);
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
    if (in != NULL )
    {
        BrainLayer input_layer = get_network_layer(network, 0);

        set_layer_input(input_layer,
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
        const BrainSettings  settings = network->_settings;

        if (settings != NULL)
        {
            const BrainUint   max_iter     = get_settings_max_iterations(settings);
            const BrainDouble target_error = get_settings_target_error(settings);
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
