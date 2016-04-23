#include "brain.h"
#include <string.h>

const Synapse*
find_synapse(Network* network, const int layer_id, const int neuron_id)
{
    int i;

    if (!network || !network->_synapses)
    {
        return NULL;
    }

    for (i = 0; i < network->_number_of_synapse; ++i)
    {
        const Synapse* synapse = network->_synapses[i];

        if (synapse->_input_layer == layer_id && synapse->_input_neuron == neuron_id)
        {
            return synapse;
        }
    }

    return NULL;
}

double
backpropagate_output_layer(Network* network, const int number_of_output, const double* desired)
{
    double error = 0.0;

    if (network && desired)
    {
        //backpropagate the error to the output layer
        Layer* output_layer = layer(network, network->_number_of_layer - 1);

        if (output_layer != NULL && output_layer->_number_of_neuron == number_of_output)
        {
            int neuron_index = 0;

            for (neuron_index = 0; neuron_index < output_layer->_number_of_neuron; ++neuron_index)
            {
                Neuron* oNeuron = neuron(output_layer, neuron_index);

                if (oNeuron != NULL)
                {
                    append_delta(oNeuron, oNeuron->_out - desired[neuron_index]);

                    error += ((desired[neuron_index] - oNeuron->_out) * (desired[neuron_index] - oNeuron->_out)) / 2.0;
                }
            }
        }
    }

    return error;
}

void
backpropagate_hidden_layer(Network* network, const int layer_index)
{
    int j;

    if (network && 0 <= layer_index && layer_index < network->_number_of_layer)
    {
        Layer* pLayer = layer(network, layer_index);

        if (pLayer != NULL)
        {
            for (j = 0; j < pLayer->_number_of_neuron; ++j)
            {
                Neuron *pNeuron = pLayer->_neurons[j];

                if (pNeuron)
                {
                    const Synapse *synapse = find_synapse(network, pLayer->_id, pNeuron->_id);

                    if (synapse)
                    {
                        Layer* nLayer = layer(network, synapse->_output_layer);

                        if (nLayer)
                        {
                            Neuron* nNeuron = neuron(nLayer, synapse->_output_neuron);

                            if (nNeuron)
                            {
                                append_delta(pNeuron, get_weighted_delta(nNeuron, synapse->_input_index));
                            }
                        }
                    }
                }
            }
        }
    }
}

void
feedforward(Network *network)
{
    int synapse_index = 0, i;
    Synapse* synapse = NULL;
    Layer *input_layer = NULL, *output_layer = NULL;
    Neuron *input_neuron = NULL, *output_neuron = NULL;

    if (network != NULL)
    {
        for (synapse_index = 0; synapse_index < network->_number_of_synapse; ++synapse_index)
        {
            synapse      = network->_synapses[synapse_index];
            input_layer  = layer(network, synapse->_input_layer);
            output_layer = layer(network, synapse->_output_layer);

            if (input_layer && output_layer)
            {
                input_neuron  = neuron(input_layer, synapse->_input_neuron);
                output_neuron = neuron(output_layer, synapse->_output_neuron);

                if (input_neuron && output_neuron)
                {
                    propagate(output_neuron, input_neuron->_out, synapse->_input_index);
                    activate(output_neuron);
                }
            }
        }

        output_layer = layer(network, network->_number_of_layer - 1);
        if (output_layer)
        {
            for (i = 0; i < output_layer->_number_of_neuron; ++i)
            {
                output_neuron = neuron(output_layer, i);

                if (output_neuron)
                {
                    network->_output[i] = output_neuron->_out;
                }
            }
        }
    }
}


double
backpropagate(Network *network, const int number_of_output, const double *desired)
{
    int i, j;
    const double error = backpropagate_output_layer(network, number_of_output, desired);

    for (i = network->_number_of_layer - 2; i >= 0; --i)
    {
        backpropagate_hidden_layer(network, i);
    }

    // now update all weight
    for (i = network->_number_of_layer - 1; i >= 0; --i)
    {
        Layer* pLayer = layer(network, i);

        if (pLayer != NULL)
        {
            for (j = 0; j < pLayer->_number_of_neuron; ++j)
            {
                Neuron *pNeuron = pLayer->_neurons[j];

                if (pNeuron)
                {
                    update(pNeuron);
                }
            }
        }
    }

    return error;
}
