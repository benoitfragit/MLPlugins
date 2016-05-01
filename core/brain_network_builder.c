#include "brain_network_builder.h"

const double*
getoutput(const Network* network)
{
    if (network != NULL)
    {
        return network->_output;
    }

    return NULL;
}


Layer_t
layer(Network* network, const int layer_index)
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

Synapse*
synapse(Network* network, const int layer_id, const int neuron_id)
{
    int i;

    if (network != NULL)
    {
        for (i = 0; i < network->_number_of_synapse; ++i)
        {
            if (network->_synapses[i]->_input_layer == layer_id && network->_synapses[i]->_input_neuron == neuron_id)
            {
                return network->_synapses[i];
            }
        }
    }

    return NULL;
}

void
delete_network(Network *network)
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
set_network_input(Network* network, const int number_of_input, const double *in)
{
    if (in != NULL )
    {
        Layer_t input_layer = layer(network, 0);

        set_layer_input(input_layer, number_of_input, in);
    }
}

Network*
new_network_from_context(Context context)
{
    Context subcontext;
    Network* _network = NULL;
    int index = 0, number_of_outputs;

    if (!context || !is_node_with_name(context, "network"))
    {
        BRAIN_CRITICAL("Network", "<%s:%d> Context is not valid !\n",  __FILE__, __LINE__);
        return NULL;
    }

    _network = (Network *)malloc(sizeof(Network));
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
        _network->_synapses = (Synapse **)malloc(_network->_number_of_synapse * sizeof(Synapse *));

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

void
dump_network(const Network* network, const char* filename)
{
	int i;

	if (network && filename)
	{
		FILE* file = fopen(filename, "w");
		if (file)
		{
			fprintf(file, "<network\n");
			fprintf(file, " trained=\"%d\"", network->_is_trained);
			fprintf(file, ">\n");

			for (i = 0; i < network->_number_of_layer; ++i)
			{
				dump_layer(network->_layers[i], file);
			}
		
			fprintf(file, "\n");

			for (i = 0; i < network->_number_of_synapse;++i)
			{
				dump_synapse(network->_synapses[i], file);
			}

			fprintf(file, "</network>\n");
			fclose(file);
		}
	}
	else
	{
		BRAIN_CRITICAL("Please give a valid file for dumping network content");
	}
}
