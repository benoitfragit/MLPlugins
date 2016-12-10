#include "brain_network_builder.h"

struct Network
{
    BrainLayer* _layers;
    BrainSignal _output;
    BrainInt    _number_of_layer;
} Network;

static void
update_network_weight(BrainNetwork network)
{
    BrainInt i;

    for (i = get_network_number_of_layer(network) - 1; i >= 0; --i)
    {
        BrainLayer pLayer = get_network_layer(network, i);

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

    if (network && desired)
    {
        const BrainInt number_of_layers = get_network_number_of_layer(network);
        BrainLayer output_layer = get_network_layer(network, number_of_layers - 1);
        const BrainSignal output = get_layer_output(output_layer);
        const BrainInt number_of_neuron = get_layer_number_of_neuron(output_layer);

        if (output_layer != NULL && number_of_neuron == number_of_output)
        {
            BrainInt output_index = 0;

            reset_layer_delta(output_layer);

            for (output_index = 0; output_index < number_of_output; ++output_index)
            {
                const BrainDouble loss = output[output_index] - desired[output_index];
                BrainNeuron oNeuron = get_layer_neuron(output_layer, output_index);

                error += loss * loss;

                if (oNeuron != NULL)
                {
                    set_neuron_delta(oNeuron, loss);
                }
            }
        }
    }

    return (error * 0.5);
}

static void
backpropagate_hidden_layer(BrainNetwork network, const BrainInt layer_index)
{
    const BrainInt number_of_layers = get_network_number_of_layer(network);

    if (network && 0 <= layer_index && layer_index < number_of_layers - 1)
    {
        BrainLayer current_layer                = get_network_layer(network, layer_index);
        const BrainLayer next_layer             = get_network_layer(network, layer_index + 1);
        const BrainInt current_number_of_neuron = get_layer_number_of_neuron(current_layer);

        if (current_layer != NULL && next_layer != NULL)
        {
            BrainInt i = 0;

            reset_layer_delta(current_layer);

            for (i = 0; i < current_number_of_neuron; ++i)
            {
                BrainNeuron current_neuron = get_layer_neuron(current_layer, i);

                if (current_neuron != NULL)
                {
                    const BrainDouble weighted_delta = get_layer_weighted_delta(next_layer, i);
                    set_neuron_delta(current_neuron, weighted_delta);
                }
            }
        }
    }
}

static BrainDouble
backpropagate(BrainNetwork network,
              const BrainInt number_of_output,
              const BrainDouble *desired)
{
    BrainInt i;
    const BrainInt number_of_layers = get_network_number_of_layer(network);
    const BrainDouble error = backpropagate_output_layer(network, number_of_output, desired);

    for (i = number_of_layers - 2; i >= 0; --i)
    {
        backpropagate_hidden_layer(network, i);
    }

    update_network_weight(network);

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

void
delete_network(BrainNetwork network)
{
    if (network != NULL)
    {
        if (network->_layers != NULL)
        {
            BrainInt i = 0;

            for (i = 0; i < network->_number_of_layer; ++i)
            {
                delete_layer(network->_layers[i]);
            }

            free(network->_layers);
        }

        free(network);
    }
}

void
set_network_input(BrainNetwork network, const BrainInt number_of_input, const BrainSignal in)
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
    if (context && is_node_with_name(context, "network"))
    {

        srand(time(NULL));

        BrainNetwork _network      = (BrainNetwork)calloc(1, sizeof(Network));
        _network->_number_of_layer = get_number_of_node_with_name(context, "layer");

        if (_network->_number_of_layer)
        {
            BrainInt index = 0;
            BrainLayer last_layer = NULL;
            const BrainInt last_layer_index = _network->_number_of_layer - 1;

            _network->_layers = (BrainLayer *)calloc(_network->_number_of_layer, sizeof(BrainLayer));

            for (index = 0; index < _network->_number_of_layer; ++index)
            {
                Context subcontext = get_node_with_name_and_index(context, "layer", index);

                if (subcontext)
                {
                    _network->_layers[index] = new_layer_from_context(subcontext);
                }
            }

            last_layer = get_network_layer(_network, last_layer_index);
            _network->_output = get_layer_output(last_layer);
        }

        return _network;
    }

    return NULL;
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

void
dump_network(const BrainNetwork network, BrainString filename)
{
    if (network != NULL && filename != NULL)
    {
        FILE* file = fopen(filename, "w");
        if (file)
        {
            BrainInt i;

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
        const BrainInt number_of_weights = get_number_of_node_with_name(context, "weight");
        BrainInt index = 0;

        for (index = 0; index < number_of_weights; ++index)
        {
            Context subcontext = get_node_with_name_and_index(context, "weight", index);

            const BrainInt   layer_idx  = node_get_int   (subcontext, "layer",  -1);
            const BrainInt   neuron_idx = node_get_int   (subcontext, "neuron", -1);
            const BrainInt   input_idx  = node_get_int   (subcontext, "input",  -1);
            const BrainDouble weight     = node_get_double(subcontext, "value",  0.0);

            BrainLayer layer = get_network_layer(network, layer_idx);

            if (layer != NULL)
            {
                BrainNeuron neuron = get_layer_neuron(layer, neuron_idx);

                if (neuron != NULL)
                {
                    const BrainInt number_of_inputs = get_neuron_number_of_inputs(neuron);
                    if (0 <= input_idx && input_idx < number_of_inputs)
                    {
                        set_neuron_weight(neuron, input_idx, weight);
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
        const BrainInt number_of_layers = get_network_number_of_layer(network);
        BrainInt index = 0;

        for (index = 1; index < number_of_layers; ++index)
        {
            BrainLayer prev = get_network_layer(network, index - 1);
            BrainLayer next = get_network_layer(network, index);

            set_layer_input(next, get_layer_number_of_neuron(prev), get_layer_output(prev));
        }
    }
}

BrainResult
train(BrainNetwork network,
      const BrainData data,
      const BrainDouble target_error,
      const BrainInt max_iter)
{
    BrainDouble error = target_error + 1.0;

    if (network && data && target_error >= 0 && max_iter >= 1)
    {
        BrainInt iteration = 0;

        do
        {
            const BrainInt index = get_data_random_subset_index(data);

            set_network_input(network, get_data_input_length(data), get_data_input(data, index));

            feedforward(network);

            error = backpropagate(network, get_data_output_length(data), get_data_output(data, index));

            ++iteration;
        } while ((iteration < max_iter) && (target_error > error));
    }

    if (error > target_error)
    {
        return BRAIN_FAILED;
    }

    return BRAIN_SUCCESS;
}
