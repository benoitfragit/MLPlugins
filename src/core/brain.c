#include "brain.h"
#include <string.h>

double
backpropagate_output_layer(Network_t network,
                           const int number_of_output,
                           const double* desired)
{
    double error = 0.0;
    int number_of_neuron = 0;

    if (network && desired)
    {
        //backpropagate the error to the output layer
        Layer_t output_layer = layer(network, get_number_of_layer(network) - 1);

        number_of_neuron = get_layer_number_of_neuron(output_layer);

        if (output_layer != NULL && number_of_neuron == number_of_output)
        {
            int neuron_index = 0;

            for (neuron_index = 0; neuron_index < number_of_neuron; ++neuron_index)
            {
                Neuron_t oNeuron = get_layer_neuron(output_layer, neuron_index);

                if (oNeuron != NULL)
                {
                    const double loss = get_neuron_output(oNeuron) - desired[neuron_index];

                    append_neuron_delta(oNeuron, loss);

                    error += loss / 2.0;
                }
            }
        }
    }

    return error;
}

void
backpropagate_hidden_layer(Network_t network, const int layer_index)
{
    int j;
    int number_of_neuron = 0;

    if (network && 0 <= layer_index && layer_index < get_number_of_layer(network))
    {
        Layer_t pLayer = layer(network, layer_index);
        number_of_neuron = get_layer_number_of_neuron(pLayer);

        if (pLayer != NULL)
        {
            for (j = 0; j < number_of_neuron; ++j)
            {
                const Synapse_t neural_synapse = synapse(network, layer_index, j);

                backpropagate_synapse(neural_synapse);
            }
        }
    }
}

void
feedforward(Network_t network)
{
    if (network != NULL)
    {
        network_activate_synapse(network);
        network_update_output(network);
    }
}

double
backpropagate(Network_t network,
              const int number_of_output,
              const double *desired)
{
    int i;
    const double error = backpropagate_output_layer(network, number_of_output, desired);

    for (i = get_number_of_layer(network) - 2; i >= 0; --i)
    {
        backpropagate_hidden_layer(network, i);
    }

    update_network_weight(network);

    BRAIN_INFO("Brain quadratic error id %lf", error);

    return error;
}

void
train(Network_t network,
      const Data_t data,
      const double target_error,
      const int max_iter)
{
    int iteration = 0, subset_index = 0, index = 0;
    double error = target_error + 1.0;

    if (network && data && target_error >= 0 && max_iter >= 1)
    {
        srand(time(NULL));

        do
        {
            subset_index = rand() % get_subset_length(data);
            index        = get_subset_index(data, subset_index);

            set_network_input(network, get_signal_length(data), get_signal(data, index));
            feedforward(network);
            error = backpropagate(network, get_observation_length(data), get_observation(data, index));

            if (target_error <= error)
            {
                BRAIN_INFO("Network has beene successfully trained");
                return;
            }

            ++iteration;
        } while ((iteration < max_iter) && (target_error > error));
    }

    if (error > target_error)
    {
        BRAIN_CRITICAL("Unable to train the neural network, target error = %lf, error = %lf", target_error, error);
        return;
    }
}
