#include "brain_network_builder.h"

struct Network
{
    BrainLayer   *_layers;
    BrainSynapse *_synapses;
    BrainDouble    *_output;

    BrainInt _number_of_synapse;
    BrainInt _number_of_layer;
} Network;

static void
set_network_output(BrainNetwork network, const BrainInt index, const BrainDouble value)
{
    if (network
    &&  (0 <= index)
    &&  (index < network->_number_of_layer))
    {
        network->_output[index] = value;
    }
}

static void
update_network_output(BrainNetwork network)
{
    BrainInt i = 0;
    BrainLayer  output_layer = NULL;
    BrainNeuron output_neuron = NULL;

    if (network != NULL)
    {
        output_layer = get_network_layer(network, get_network_number_of_layer(network) - 1);

        if (output_layer)
        {
            for (i = 0; i < get_layer_number_of_neuron(output_layer); ++i)
            {
                output_neuron = get_layer_neuron(output_layer, i);

                if (output_neuron)
                {
                    set_network_output(network, i, get_neuron_output(output_neuron));
                }
            }
        }
    }
}

static BrainSynapse
get_network_synapse_with_index(BrainNetwork network, const BrainInt index)
{
    if (network
    && (0 <= index)
    && (index < network->_number_of_synapse)
    && is_synapse_valid(network->_synapses[index]))
    {
        return network->_synapses[index];
    }

    return NULL;
}

static void
activate_network_synapse(BrainNetwork network)
{
    BrainInt synapse_index       = 0;
    BrainSynapse synapse       = NULL;;

    if (network != NULL)
    {
        for (synapse_index = 0; synapse_index < get_network_number_of_synapse(network); ++synapse_index)
        {
            synapse = get_network_synapse_with_index(network, synapse_index);;

            activate_synapse(synapse);
        }
    }
}

static void
update_network_weight(BrainNetwork network)
{
    BrainInt i;
    BrainLayer pLayer = NULL;

    for (i = get_network_number_of_layer(network) - 1; i >= 0; --i)
    {
        pLayer = get_network_layer(network, i);

        if (pLayer != NULL)
        {
            update_layer_weight(pLayer);
        }
    }
}

static BrainDouble
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

static void
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

static BrainSynapse
new_network_synapse_from_context(BrainNetwork network, Context context)
{
    BrainInt input_layer_idx;
    BrainInt input_neuron_idx;
    BrainInt output_layer_idx;
    BrainInt output_neuron_idx;
    BrainInt input_index;

    BrainLayer  input_layer   = NULL;
    BrainLayer  output_layer  = NULL;
    BrainNeuron input_neuron  = NULL;
    BrainNeuron output_neuron = NULL;

    if (network != NULL)
    {
        BrainSynapse _synapse = NULL;

        if (!context || !is_node_with_name(context, "connect"))
        {
            return NULL;
        }

        input_layer_idx   = node_get_int(context, "input-layer",   0);
        output_layer_idx  = node_get_int(context, "output-layer",  0);
        input_neuron_idx  = node_get_int(context, "input-neuron",  0);
        output_neuron_idx = node_get_int(context, "output-neuron", 0);
        input_index       = node_get_int(context, "input-index",   0);

        input_layer   = get_network_layer(network, input_layer_idx);
        output_layer  = get_network_layer(network, output_layer_idx);
        input_neuron  = get_layer_neuron(input_layer, input_neuron_idx);
        output_neuron = get_layer_neuron(output_layer, output_neuron_idx);

        if (input_layer && output_layer && input_neuron && output_neuron)
        {
            _synapse = new_synapse();

            set_synapse_input_index        (_synapse, input_index);
            set_synapse_input_neuron       (_synapse, input_neuron);
            set_synapse_output_neuron      (_synapse, output_neuron);
            set_synapse_input_layer_index  (_synapse, input_layer_idx);
            set_synapse_output_layer_index (_synapse, output_layer_idx);
            set_synapse_input_neuron_index (_synapse, input_neuron_idx);
            set_synapse_output_neuron_index(_synapse, output_neuron_idx);

            return _synapse;
        }
        else
        {
            delete_synapse(_synapse);
        }
    }

    return NULL;
}

BrainDouble*
get_network_output(const BrainNetwork network)
{
    if (network != NULL)
    {
        return network->_output;
    }

    return NULL;
}

BrainLayer
get_network_layer(BrainNetwork network, const BrainInt layer_index)
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

BrainSynapse
get_network_synapse(BrainNetwork network, const BrainInt layer_idx, const BrainInt neuron_idx)
{
    BrainInt i;

    if (network != NULL)
    {
        for (i = 0; i < network->_number_of_synapse; ++i)
        {
            if (is_synapse_valid(network->_synapses[i])
            &&  (get_synapse_input_layer_index (network->_synapses[i]) == layer_idx)
            &&  (get_synapse_input_neuron_index(network->_synapses[i]) == neuron_idx))
            {
                return network->_synapses[i];
            }
        }
    }

    return NULL;
}

void
delete_network(BrainNetwork network)
{
    if (network != NULL)
    {
        BrainInt i;
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
set_network_input(BrainNetwork network, const BrainInt number_of_input, const BrainDouble *in)
{
    if (in != NULL )
    {
        BrainLayer input_layer = get_network_layer(network, 0);

        set_layer_input(input_layer, number_of_input, in);
    }
}

BrainNetwork
new_network_from_context(Context context)
{
    Context subcontext;
    BrainNetwork _network = NULL;
    BrainInt index = 0, number_of_outputs;

    if (!context || !is_node_with_name(context, "network"))
    {
        return NULL;
    }

    srand(time(NULL));

    _network = (BrainNetwork)malloc(sizeof(Network));
    _network->_number_of_layer   = get_number_of_node_with_name(context, "layer");
    _network->_number_of_synapse = get_number_of_node_with_name(context, "connect");

    if (_network->_number_of_layer)
    {
        _network->_layers = (BrainLayer *)malloc(_network->_number_of_layer * sizeof(BrainLayer));

        for (index = 0; index < _network->_number_of_layer; ++index)
        {
            subcontext = get_node_with_name_and_index(context, "layer", index);

            if (subcontext)
            {
                _network->_layers[index] = new_layer_from_context(subcontext);

                number_of_outputs = get_layer_number_of_neuron(_network->_layers[index]);
            }
        }
    }

    if (_network->_number_of_synapse)
    {
        _network->_synapses = (BrainSynapse *)malloc(_network->_number_of_synapse * sizeof(BrainSynapse));

        for (index = 0; index < _network->_number_of_synapse; ++index)
        {
            subcontext = get_node_with_name_and_index(context, "connect", index);

            if (subcontext)
            {
                _network->_synapses[index] = new_network_synapse_from_context(_network, subcontext);
            }
        }
    }

    _network->_output = (BrainDouble *)malloc( number_of_outputs * sizeof(BrainDouble));
    memset(_network->_output, 0, number_of_outputs * sizeof(BrainDouble));

    return _network;
}

BrainInt
get_network_number_of_layer(const BrainNetwork network)
{
    if (network)
    {
        return network->_number_of_layer;
    }

    return 0;
}

BrainInt
get_network_number_of_synapse(const BrainNetwork network)
{
    if (network)
    {
        return network->_number_of_synapse;
    }

    return 0;
}

void
dump_network(const BrainNetwork network, BrainString filename)
{
    BrainInt i;

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
}

void
initialize_network_from_context(BrainNetwork network, Context context)
{
    if (network != NULL)
    {
        BrainInt index, layer_idx, neuron_idx, input_idx;
        BrainDouble weight;

        Context subcontext;

        BrainLayer  BrainInternal_layer  = NULL;
        BrainNeuron BrainInternal_neuron = NULL;

        const BrainInt number_of_weights = get_number_of_node_with_name(context, "weight");

        for (index = 0; index < number_of_weights; ++index)
        {
            subcontext = get_node_with_name_and_index(context, "weight", index);

            layer_idx  = node_get_int   (subcontext, "layer",  -1);
            neuron_idx = node_get_int   (subcontext, "neuron", -1);
            input_idx  = node_get_int   (subcontext, "input",  -1);
            weight     = node_get_double(subcontext, "value",  0.0);

            BrainInternal_layer     = get_network_layer(network, layer_idx);

            if (BrainInternal_layer != NULL)
            {
                BrainInternal_neuron = get_layer_neuron(BrainInternal_layer, neuron_idx);

                if (BrainInternal_neuron != NULL)
                {
                    if (0 <= input_idx && input_idx < get_neuron_number_of_inputs(BrainInternal_neuron))
                    {
                        set_neuron_weight(BrainInternal_neuron, input_idx, weight);
                    }
                }
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
      const BrainData data,
      const BrainDouble target_error,
      const BrainInt max_iter)
{
    BrainInt iteration = 0, index = 0;
    BrainDouble error = target_error + 1.0;

    if (network && data && target_error >= 0 && max_iter >= 1)
    {
        do
        {
            index = get_data_random_subset_index(data);
            set_network_input(network, get_data_input_length(data), get_data_input(data, index));
            feedforward(network);
            error = backpropagate(network, get_data_output_length(data), get_data_output(data, index));

            ++iteration;
        } while ((iteration < max_iter) && (target_error > error));
    }

    if (error > target_error)
    {
        return BRAIN_SUCCESS;
    }

    return BRAIN_FAILED;
}
