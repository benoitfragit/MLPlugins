#include "brain_network_builder.h"

struct Network
{
    Layer_t   *_layers;
    Synapse_t *_synapses;
    double    *_output;

    int _is_trained;

    int _number_of_synapse;
    int _number_of_layer;
} Network;

void
network_update_output(Network_t network)
{
    int i = 0;
    Layer_t output_layer = NULL;
    Neuron_t output_neuron = NULL;

    if (network != NULL)
    {
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

void
network_propagate_synapse(Network_t network)
{
    int synapse_index       = 0;
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
    }
}

void
set_trained(Network_t network, const int trained)
{
    if (network)
    {
        network->_is_trained = trained;
    }
}

int
is_trained(const Network_t network)
{
    if (network)
    {
        return network->_is_trained;
    }

    return 0;
}

void
set_output(Network_t network, const int index, const double value)
{
    if (network
    &&  (0 <= index)
    &&  (index < network->_number_of_layer))
    {
        network->_output[index] = value;
    }
}

const double*
get_output(const Network_t network)
{
    if (network != NULL)
    {
        return network->_output;
    }

    return NULL;
}

Layer_t
layer(Network_t network, const int layer_index)
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

Synapse_t
synapse(Network_t network, const int layer_id, const int neuron_id)
{
    int i;

    if (network != NULL)
    {
        for (i = 0; i < network->_number_of_synapse; ++i)
        {
            if (get_input_layer(network->_synapses[i]) == layer_id && get_input_neuron(network->_synapses[i]) == neuron_id)
            {
                return network->_synapses[i];
            }
        }
    }

    return NULL;
}

Synapse_t
synapse_with_index(Network_t network, const int index)
{
    if (network
    && (0 <= index)
    && (index < network->_number_of_synapse))
    {
        return network->_synapses[index];
    }

    return NULL;
}

void
delete_network(Network_t network)
{
    if (network != NULL)
    {
        int i;
        if (network->_number_of_synapse > 0)
        {
            for (i = 0; i < network->_number_of_synapse; ++i)
            {
                delete_synapse(network->_synapses[i]);
            }

            free(network->_synapses);
        }

        if (network->_layers != NULL)
        {
            for (i = 0; i < network->_number_of_layer; ++i)
            {
                delete_layer(network->_layers[i]);
            }

            free(network->_layers);
        }

        if (network->_output)
        {
            free(network->_output);
        }

        free(network);
    }
}

void
set_network_input(Network_t network, const int number_of_input, const double *in)
{
    if (in != NULL )
    {
        Layer_t input_layer = layer(network, 0);

        set_layer_input(input_layer, number_of_input, in);
    }
}

Network_t
new_network_from_context(Context context)
{
    Context subcontext;
    Network_t _network = NULL;
    int index = 0, number_of_outputs;

    if (!context || !is_node_with_name(context, "network"))
    {
        BRAIN_CRITICAL("Network", "<%s:%d> Context is not valid !\n",  __FILE__, __LINE__);
        return NULL;
    }

    _network = (Network_t)malloc(sizeof(Network));
    _network->_number_of_layer   = get_number_of_node_with_name(context, "layer");
    _network->_number_of_synapse = get_number_of_node_with_name(context, "connect");
    _network->_is_trained        = node_get_int(context, "trained", 0);

    BRAIN_INFO("Number of layer : %d, Number of synapse : %d", _network->_number_of_layer,
                                                                                 _network->_number_of_synapse);

    if (_network->_number_of_layer)
    {
        _network->_layers = (Layer_t *)malloc(_network->_number_of_layer * sizeof(Layer_t));

        for (index = 0; index < _network->_number_of_layer; ++index)
        {
            subcontext = get_node_with_name_and_index(context, "layer", index);

            if (subcontext)
            {
                _network->_layers[index] = new_layer_from_context(subcontext);
            }
        }
    }

    if (_network->_number_of_synapse)
    {
        _network->_synapses = (Synapse_t *)malloc(_network->_number_of_synapse * sizeof(Synapse_t));

        for (index = 0; index < _network->_number_of_synapse; ++index)
        {
            subcontext = get_node_with_name_and_index(context, "connect", index);

            if (subcontext)
            {
                _network->_synapses[index] = new_synapse_from_context(subcontext);
            }
        }
    }

    number_of_outputs = get_number_of_neuron(layer(_network, _network->_number_of_layer - 1));
    _network->_output = (double *)malloc( number_of_outputs * sizeof(double));
    memset(_network->_output, 0, number_of_outputs * sizeof(double));

    return _network;
}

int
get_number_of_layer(const Network_t network)
{
    if (network)
    {
        return network->_number_of_layer;
    }

    return 0;
}

int
get_number_of_synapse(const Network_t network)
{
    if (network)
    {
        return network->_number_of_synapse;
    }

    return 0;
}

void
update_network_weight(Network_t network)
{
    int i;
    Layer_t pLayer = NULL;

    for (i = get_number_of_layer(network) - 1; i >= 0; --i)
    {
        pLayer = layer(network, i);

        if (pLayer != NULL)
        {
            update_layer_weight(pLayer);
        }
    }
}

void
dump_network(const Network_t network, const char* filename)
{
    int i;

    if (network && filename)
    {
        FILE* file = fopen(filename, "w");
        if (file)
        {
            fprintf(file, "<init>\n");

            for (i = 0; i < network->_number_of_layer; ++i)
            {
                dump_layer(network->_layers[i], i, file);
            }

            fprintf(file, "</init>\n");
            fclose(file);
        }
    }
    else
    {
        BRAIN_CRITICAL("Please give a valid file for dumping network content");
    }
}

void
initialize_network_from_context(Network_t network, Context context)
{
    if (network != NULL)
    {
        int index, layer_idx, neuron_idx, input_idx;
        double weight;

        Context subcontext;

        Layer_t  internal_layer  = NULL;
        Neuron_t internal_neuron = NULL;

        const int number_of_weights = get_number_of_node_with_name(context, "weight");

        for (index = 0; index < number_of_weights; ++index)
        {
            subcontext = get_node_with_name_and_index(context, "weight", index);

            layer_idx  = node_get_int   (subcontext, "layer",  -1);
            neuron_idx = node_get_int   (subcontext, "neuron", -1);
            input_idx  = node_get_int   (subcontext, "input",  -1);
            weight     = node_get_double(subcontext, "value",  0.0);

            internal_layer     = layer(network, layer_idx);

            if (internal_layer != NULL)
            {
                internal_neuron = neuron(internal_layer, neuron_idx);

                if (internal_neuron != NULL)
                {
                    if (0 <= input_idx && input_idx < get_number_of_inputs(internal_neuron))
                    {
                        set_neuron_weight(internal_neuron, input_idx, weight);
                    }
                    else
                    {
                        BRAIN_CRITICAL("There is no input at idx: ", input_idx);
                    }
                }
                else
                {
                    BRAIN_CRITICAL("There is no neuron at idx: ", neuron_idx);
                }
            }
            else
            {
                BRAIN_CRITICAL("There is no layer at idx: ", layer_idx);
            }
        }
    }
    else
    {
        BRAIN_CRITICAL("Network is not valid");
    }
}
