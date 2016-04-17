#include "brain.h"
#include <string.h>

void
neuron_activate(Neuron *neuron)
{
    int j;
    if (neuron != NULL)
    {
        double sum = 0.0;

        for (j = 0; j < neuron->_number_of_input + 1; ++j)
        {
            sum += neuron->_in[j] * neuron->_w[j];
        }

        neuron->_out = (double)((1.0 / (1.0 + exp(-sum))) > 0.5);

        neuron->_delta = 0.0;
    }
}

void
neuron_connect(Synapse* synapse, Neuron* first, Neuron* second)
{
    if (synapse != NULL && first != NULL && second != NULL)
    {
        synapse->_neuron = second;

        if (synapse->_neuron->_number_of_input <= synapse->_number_of_connected_neuron)
        {
            if (synapse->_connected_neurons == NULL)
            {
                synapse->_number_of_connected_neuron = 1;
                synapse->_connected_neurons = (Neuron **)malloc(synapse->_number_of_connected_neuron * sizeof(Neuron *));
            }
            else
            {
                ++synapse->_number_of_connected_neuron;
                synapse->_connected_neurons = (Neuron **)realloc(synapse->_connected_neurons, synapse->_number_of_connected_neuron * sizeof(Neuron *));
            }

            synapse->_connected_neurons[synapse->_number_of_connected_neuron - 1] = first;
        }
    }
}

void
neuron_update_weight(Neuron* neuron)
{
    int index = 0;

    if (neuron != NULL)
    {
        for (index = 0; index < neuron->_number_of_input; ++index)
        {
            double correction = - neuron->_learning_rate * neuron->_delta * neuron->_in[index] + neuron->_inertial_factor * neuron->_correction[index];

            neuron->_w[index] += correction;
            neuron->_correction[index] = correction;
        }
    }
}

Layer*
get_internal_layer(Network* network, const int layer_index)
{
    if (network != NULL
    && network->_layers != NULL
    && 0 <= layer_index
    && layer_index < network->_number_of_layer)
    {
        return network->_layers[layer_index];
    }

    return NULL;
}

Neuron*
get_internal_neuron(Layer* layer, const int neuron_index)
{
    if (layer != NULL
    &&  0 <= neuron_index
    && neuron_index < layer->_number_of_neuron)
    {
        return layer->_neurons[neuron_index];
    }

    return NULL;
}

double*
get_internal_input(Neuron* neuron, const int input_index)
{
    if (neuron != NULL
    && 0 <= input_index
    && input_index < neuron->_number_of_input)
    {
        return &(neuron->_in[input_index]);
    }

    return NULL;
}

double*
get_internal_weight(Neuron* neuron, const int weight_index)
{
    if (neuron != NULL
    && 0 <= weight_index
    && weight_index < neuron->_number_of_input)
    {
        return &(neuron->_w[weight_index]);
    }

    return NULL;
}

void
delete_neuron(Neuron* neuron)
{
    if (neuron)
    {
        if (neuron->_in != NULL)
        {
            free(neuron->_in);
        }

        if (neuron->_w != NULL)
        {
            free(neuron->_w);
        }

        if (neuron->_correction != NULL)
        {
            free(neuron->_correction);
        }

        free(neuron);
    }
}

Neuron*
create_neuron(const int input)
{
    int j;
    Neuron *neuron = (Neuron *)malloc(sizeof(Neuron));

    srand(time(NULL));

    //don't forget to add one because of the bias
    size_t total_size = (input + 1) * sizeof(double);
    neuron->_in = (double *)malloc(total_size);
    neuron->_w  = (double *)malloc(total_size);
    neuron->_correction = (double *)malloc(total_size);
    neuron->_out = 0.0;
    neuron->_number_of_input = input;

    for (j = 0; j < neuron->_number_of_input; ++ j)
    {
        neuron->_w[j] = rand() / (double)RAND_MAX;
        neuron->_in[j] = 0.0;
    }

    //set all correction to 0
    memset(neuron->_correction, 0, total_size);

    //hidden input to simulate the bias on this neuron
    neuron->_in[input] = -1.0;
    neuron->_w[input] = rand() / (double)RAND_MAX;

    neuron->_learning_rate = 1.0;
    neuron->_inertial_factor = 0.0;
    neuron->_delta = 0.0;

    return neuron;
}

Layer*
create_layer(const int n, const int input[])
{
    int i;
    Layer* layer = (Layer *)malloc(sizeof(Layer));
    layer->_number_of_neuron = n;

    layer->_neurons = (Neuron **)malloc(n * sizeof(Neuron *));

    for (i = 0; i < n; ++i)
    {
        layer->_neurons[i] = create_neuron(input[i]);
    }

    return layer;
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

Network*
createNetwork(const int m, int *n, int **o)
{
    int i;

    Network* network = (Network *)malloc(sizeof(Network));
    network->_layers = (Layer **)malloc(m * sizeof(Layer *));
    network->_synapses = NULL;
    network->_number_of_synapse = 0;
    network->_number_of_layer = 0;

    for (i = 0; i < m; ++i)
    {
        network->_layers[i] = create_layer(n[i], *(o + i));
    }

    network->_output = (double *)malloc(n[m-1] * sizeof(double));
    memset(network->_output, 0, n[m-1]*sizeof(double));

    return network;
}

void
deleteNetwork(Network *network)
{
    if (network != NULL)
    {
        int i;
        if (network->_layers != NULL)
        {
            for (i = 0; i < network->_number_of_layer; ++i)
            {
                delete_layer(network->_layers[i]);
            }

            free(network->_layers);
        }

        if (network->_synapses != NULL)
        {
            //synapse is not the owner of its content;
            for (i = 0; i < network->_number_of_synapse; ++i)
            {
                if (network->_synapses[i]->_connected_neurons)
                {
                    free(network->_synapses[i]->_connected_neurons);
                }
            }

            free(network->_synapses);
        }

        if (network->_output)
        {
            free(network->_output);
        }

        free(network);
    }
}

Synapse*
get_internal_synapse(Network* network, Neuron* neuron)
{
    int i;

    if (network != NULL && neuron != NULL)
    {
        for (i = 0; i < network->_number_of_synapse; ++i)
        {
            if (network->_synapses[i]->_neuron != NULL && network->_synapses[i]->_neuron == neuron)
            {
                return network->_synapses[i];
            }
        }
    }

    return NULL;
}

int
connect(Network* network,
               const int first_layer,
               const int second_layer,
               const int first_neuron,
               const int second_neuron,
               const int input_index)
{
    if (network != NULL)
    {
        Layer* fLayer = get_internal_layer(network, first_layer);
        Layer* sLayer = get_internal_layer(network, second_layer);

        if (fLayer != NULL && sLayer != NULL)
        {
            Neuron* fNeuron = get_internal_neuron(fLayer, first_neuron);
            Neuron* sNeuron = get_internal_neuron(sLayer, second_neuron);

            Synapse *synapse = NULL;

            if (network->_synapses == NULL)
            {
                network->_synapses = (Synapse **)malloc(sizeof(Synapse *));
                network->_number_of_synapse = 1;

                synapse = network->_synapses[network->_number_of_synapse - 1];
            }
            else
            {
                if (sNeuron != NULL)
                {
                    synapse = get_internal_synapse(network, sNeuron);
                }

                if (synapse == NULL)
                {
                    ++network->_number_of_synapse;
                    network->_synapses = (Synapse **)realloc(network->_synapses, network->_number_of_synapse*sizeof(Synapse *));

                    synapse = network->_synapses[network->_number_of_synapse - 1];
                }
            }

            if (synapse != NULL && fNeuron != NULL && sNeuron != NULL)
            {
                neuron_connect(synapse, fNeuron, sNeuron);
            }

            return 1;
        }
    }

    return 0;
}

void
synapse_feed(Synapse* synapse)
{
    int i;
    for (i = 0; i < synapse->_number_of_connected_neuron; ++i)
    {
        synapse->_neuron->_in[i] = synapse->_connected_neurons[i]->_out;

        neuron_activate(synapse->_neuron);
    }
}

void
feed(Network *network)
{
    int synapse_index = 0;
    if (network != NULL)
    {
        for (synapse_index = 0; synapse_index < network->_number_of_synapse; ++synapse_index)
        {
            synapse_feed(network->_synapses[synapse_index]);
        }
    }
}

void
synapse_update_delta(Synapse* synapse)
{
    if (synapse != NULL)
    {
        int i = 0;

        for (i = 0; i < synapse->_number_of_connected_neuron; ++i)
        {
            synapse->_connected_neurons[i]->_delta += synapse->_neuron->_delta * synapse->_neuron->_w[i] * synapse->_connected_neurons[i]->_out * (1.0 - synapse->_connected_neurons[i]->_out);
        }
    }
}

double
forward(Network *network, const int number_of_output, const double *desired)
{
    int i, j;
    double error = 0.0;

    //forward propagation on the output layer
    Layer* output_layer = get_internal_layer(network, network->_number_of_layer - 1);

    if (output_layer != NULL && output_layer->_number_of_neuron == number_of_output)
    {
        int neuron_index = 0;
        double delta = 0.0;

        for (neuron_index = 0; neuron_index < output_layer->_number_of_neuron; ++neuron_index)
        {
            Neuron* neuron = get_internal_neuron(output_layer, neuron_index);

            network->_output[neuron_index] = neuron->_out;

            if (neuron != NULL)
            {
                delta = neuron->_out * (1.0 - neuron->_out) * (desired[i] - neuron->_out);

                error += ((desired[i] - neuron->_out) * (desired[i] - neuron->_out)) / 2.0;

                neuron->_delta = delta;
                neuron_update_weight(neuron);

                Synapse* synapse = get_internal_synapse(network, neuron);

                synapse_update_delta(synapse);
            }
        }
    }

    for (i = network->_number_of_layer - 2; i >= 0; --i)
    {
        Layer* pLayer = get_internal_layer(network, i);

        if (pLayer != NULL)
        {
            for (j = 0; j < pLayer->_number_of_neuron; ++j)
            {
                Neuron *neuron = pLayer->_neurons[j];
                Synapse *synapse = get_internal_synapse(network, neuron);

                neuron_update_weight(neuron);
                synapse_update_delta(synapse);
            }
        }
    }

    return error;
}

void
setInput(Network* network, const int number_of_input, const double *in)
{
    int j, k;

    if (in != NULL )
    {
        Layer* input_layer = get_internal_layer(network, 0);

        if (input_layer)
        {
            for (j = 0; j < input_layer->_number_of_neuron; ++j)
            {
                Neuron* neuron = input_layer->_neurons[j];

                if (neuron != NULL && neuron->_number_of_input == number_of_input)
                {
                    for (k = 0; k < neuron->_number_of_input; ++k)
                    {
                        neuron->_in[k] = in[k];
                    }

                    neuron_activate(neuron);
                }
            }
        }
    }
}

const double*
getOutput(const Network* network)
{
    if (network != NULL)
    {
        return network->_output;
    }

    return NULL;
}
