#include "brain.h"
#include <string.h>

BrainDouble
backpropagate_output_layer(BrainNetwork network,
                           const BrainInt number_of_output,
                           const BrainDouble* desired)
{
    BrainDouble error = 0.0;
    BrainInt number_of_neuron = 0;

    if (network && desired)
    {
        BrainLayer output_layer = get_network_layer(network, get_network_number_of_layer(network) - 1);

        number_of_neuron = get_layer_number_of_neuron(output_layer);

        if (output_layer != NULL && number_of_neuron == number_of_output)
        {
            BrainInt neuron_index = 0;

            for (neuron_index = 0; neuron_index < number_of_neuron; ++neuron_index)
            {
                BrainNeuron oNeuron = get_layer_neuron(output_layer, neuron_index);

                if (oNeuron != NULL)
                {
                    const BrainDouble loss = get_neuron_output(oNeuron) - desired[neuron_index];

                    append_neuron_delta(oNeuron, loss);

                    error += loss / 2.0;
                }
            }
        }
    }

    return error;
}

void
backpropagate_hidden_layer(BrainNetwork network, const BrainInt layer_index)
{
    BrainInt j;
    BrainInt number_of_neuron = 0;

    if (network && 0 <= layer_index && layer_index < get_network_number_of_layer(network))
    {
        BrainLayer pLayer = get_network_layer(network, layer_index);
        number_of_neuron = get_layer_number_of_neuron(pLayer);

        if (pLayer != NULL)
        {
            for (j = 0; j < number_of_neuron; ++j)
            {
                const BrainSynapse neural_synapse = get_network_synapse(network, layer_index, j);

                backpropagate_synapse(neural_synapse);
            }
        }
    }
}

void
feedforward(BrainNetwork network)
{
    if (network != NULL)
    {
        activate_network_synapse(network);
        update_network_output(network);
    }
}

BrainDouble
backpropagate(BrainNetwork network,
              const BrainInt number_of_output,
              const BrainDouble *desired)
{
    BrainInt i;
    const BrainDouble error = backpropagate_output_layer(network, number_of_output, desired);

    for (i = get_network_number_of_layer(network) - 2; i >= 0; --i)
    {
        backpropagate_hidden_layer(network, i);
    }

    update_network_weight(network);

    return error;
}

BrainResult
train(BrainNetwork network,
      const Data_t data,
      const BrainDouble target_error,
      const BrainInt max_iter)
{
    BrainInt iteration = 0, subset_index = 0, index = 0;
    BrainDouble error = target_error + 1.0;

    if (network && data && target_error >= 0 && max_iter >= 1)
    {
        do
        {
            subset_index = rand() % get_subset_length(data);
            index        = get_subset_index(data, subset_index);

            set_network_input(network, get_signal_length(data), get_signal(data, index));
            feedforward(network);
            error = backpropagate(network, get_observation_length(data), get_observation(data, index));

            ++iteration;
        } while ((iteration < max_iter) && (target_error > error));
    }

    if (error > target_error)
    {
        return BRAIN_SUCCESS;
    }

    return BRAIN_FAILED;
}
