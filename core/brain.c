#include "brain.h"
#include <string.h>

double
backpropagate_output_layer(Network_t network, const int number_of_output, const double* desired)
{
    double error = 0.0;
	int number_of_neuron = 0;

    if (network && desired)
    {
        //backpropagate the error to the output layer
        Layer_t output_layer = layer(network, get_number_of_layer(network) - 1);
		
		number_of_neuron = get_number_of_neuron(output_layer);

        if (output_layer != NULL && number_of_neuron == number_of_output)
        {
            int neuron_index = 0;

            for (neuron_index = 0; neuron_index < number_of_neuron; ++neuron_index)
            {
                Neuron_t oNeuron = neuron(output_layer, neuron_index);

                if (oNeuron != NULL)
                {
                    append_delta(oNeuron, output(oNeuron) - desired[neuron_index]);

                    error += ((desired[neuron_index] - output(oNeuron)) * (desired[neuron_index] - output(oNeuron))) / 2.0;
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
		number_of_neuron = get_number_of_neuron(pLayer);

        if (pLayer != NULL)
        {
            for (j = 0; j < number_of_neuron; ++j)
            {
                Neuron_t pNeuron = neuron(pLayer, j);

                if (pNeuron)
                {
                    const Synapse_t neural_synapse = synapse(network, get_layer_id(pLayer), get_neuron_id(pNeuron));

                    if (neural_synapse)
                    {
                        Layer_t nLayer = layer(network, get_output_layer(neural_synapse));

                        if (nLayer)
                        {
                            Neuron_t nNeuron = neuron(nLayer, get_output_neuron(neural_synapse));

                            if (nNeuron)
                            {
                                append_delta(pNeuron, get_weighted_delta(nNeuron, get_input_index(neural_synapse)));
                            }
                        }
                    }
                }
            }
        }
    }
}

void
feedforward(Network_t network)
{
    int synapse_index = 0, i;
    
	Synapse_t synapse       = NULL;
    Layer_t   input_layer   = NULL;
	Layer_t   output_layer  = NULL;
    Neuron_t  input_neuron  = NULL;
	Neuron_t  output_neuron = NULL;

    if (network != NULL)
    {
        for (synapse_index = 0; synapse_index < get_number_of_synapse(network); ++synapse_index)
        {
            synapse      = synapse_with_index(network, synapse_index);;
            input_layer  = layer(network, get_input_layer(synapse));
            output_layer = layer(network, get_output_layer(synapse));

            if (input_layer && output_layer)
            {
                input_neuron  = neuron(input_layer, get_input_neuron(synapse));
                output_neuron = neuron(output_layer, get_output_neuron(synapse));

                if (input_neuron && output_neuron)
                {
                    propagate(output_neuron, output(input_neuron), get_input_index(synapse));
                    activate(output_neuron);
                }
            }
        }

        output_layer = layer(network, get_number_of_layer(network) - 1);
        if (output_layer)
        {
            for (i = 0; i < get_number_of_neuron(output_layer); ++i)
            {
                output_neuron = neuron(output_layer, i);

                if (output_neuron)
                {
                    set_output(network, i, output(output_neuron));
                }
            }
        }
    }
}

double
backpropagate(Network_t network, const int number_of_output, const double *desired)
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
train(Network_t network, const Data_t data, const double target_error, const int max_iter)
{
	int iteration = 0, subset_index = 0, index = 0;
	double error = target_error + 1.0;
	
	set_trained(network, 0);
	
	if (network && data && target_error >= 0 && max_iter >= 1)
	{
		do 
		{
			for (subset_index = 0; subset_index < get_subset_length(data); ++subset_index)
			{
				index = get_subset_index(data, subset_index);
				
				set_network_input(network, get_signal_length(data), get_signal(data, index));
				feedforward(network);
				error = backpropagate(network, get_observation_length(data), get_observation(data, index));

				if (target_error <= error)
				{
					BRAIN_INFO("Network has beene successfully trained");
					set_trained(network, 1);
					return;
				}
			}
			
			++iteration;
		} while ((iteration < max_iter) && (target_error > error));
	}
	
	if (error > target_error)
	{
		BRAIN_CRITICAL("Unable to train the neural network, target error = %lf, error = %lf", target_error, error);
		set_trained(network, 0);
		return;
	}

	set_trained(network, 1);
}
