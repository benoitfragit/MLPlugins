#include "brain_network_builder.h"
#include "brain_costfunction.h"
#include "brain_layer_builder.h"
#include "brain_neuron_builder.h"
#include "brain_data_reader.h"
#include "brain_logging_utils.h"

/**
 * \struct Network
 * \brief  Internal model for a BrainNetwork
 *
 * All protected fields for a BrainNetwork
 */
struct Network
{
    BrainLayer* _layers;                   /*!< An array of BrainLayer         */
    BrainSignal _output;                   /*!< Output signal of the layer     */
    BrainUint   _number_of_layer;          /*!< Number of layers               */
    BrainDouble _dropout_percent;          /*!< Dropout per cent               */
    BrainBool   _use_dropout;              /*!< Enable or disable dropout      */
    CostPtrFunc _cost_function;            /*!< CostFunction to use            */
    CostPtrFunc _cost_function_derivative; /*!< CostFunction derivative to use */
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

static BrainLayer
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
        const BrainUint   number_of_layers = get_network_number_of_layer(network);
        BrainLayer        output_layer     = get_network_layer(network, number_of_layers - 1);
        const BrainSignal output           = get_layer_output(output_layer);
        const BrainUint   number_of_neuron = get_layer_number_of_neuron(output_layer);

        if (output_layer != NULL && number_of_neuron == number_of_output)
        {
            BrainUint output_index = 0;

            reset_layer_delta(output_layer);

            for (output_index = 0; output_index < number_of_output; ++output_index)
            {
                const BrainDouble loss = network->_cost_function_derivative(output[output_index], desired[output_index]);
                BrainNeuron oNeuron = get_layer_neuron(output_layer, output_index);

                error += network->_cost_function(output[output_index], desired[output_index]);

                if (oNeuron != NULL)
                {
                    set_neuron_delta(oNeuron, loss);
                }
            }
        }
    }

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
        BrainLayer current_layer                = get_network_layer(network, layer_index);
        const BrainLayer next_layer             = get_layer_next_layer(current_layer);
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

        free(network);
    }
}

BrainNetwork
new_network_from_context(Context context)
{
    if (context && is_node_with_name(context, "network"))
    {
        BrainChar* buffer = NULL;
        BrainCostFunctionType cost_function_type = Invalid_CostFunction;

        BrainNetwork _network      = (BrainNetwork)calloc(1, sizeof(Network));
        _network->_number_of_layer = get_number_of_node_with_name(context, "layer");
        buffer                     = (BrainChar *)node_get_prop(context, "cost-function-type");

        cost_function_type                  = get_cost_function_type(buffer);
        _network->_cost_function            = get_cost_function(cost_function_type);
        _network->_cost_function_derivative = get_cost_function_derivative(cost_function_type);

        _network->_use_dropout              = node_get_int(context, "use-dropout", 0);
        _network->_dropout_percent          = node_get_double(context, "dropout-percent", 0.5);

        if (buffer != NULL)
        {
            free(buffer);
        }

        srand(time(NULL));

        if (_network->_number_of_layer != 0)
        {
            BrainUint                  index = 0;
            BrainLayer            last_layer = NULL;
            const BrainUint last_layer_index = _network->_number_of_layer - 1;

            _network->_layers = (BrainLayer *)calloc(_network->_number_of_layer, sizeof(BrainLayer));

            for (index = 0; index < _network->_number_of_layer; ++index)
            {
                Context subcontext = get_node_with_name_and_index(context, "layer", index);

                if (subcontext)
                {
                    _network->_layers[index] = new_layer_from_context(subcontext);

                    if (0 < index && _network->_layers[index] != NULL)
                    {
                        set_layer_next_layer(_network->_layers[index - 1], _network->_layers[index]);
                    }
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
initialize_network_from_context(BrainNetwork network,
                                Context context)
{
    if (network != NULL)
    {
        const BrainUint number_of_neurons = get_number_of_node_with_name(context, "neuron");
        BrainUint index = 0;

        network->_use_dropout     = BRAIN_FALSE;
        network->_dropout_percent = 0.0;

        for (index = 0; index < number_of_neurons; ++index)
        {
            Context subcontext = get_node_with_name_and_index(context, "neuron", index);

            const BrainUint layer_idx         = node_get_int(subcontext, "layer-index",  -1);
            const BrainUint neuron_idx        = node_get_int(subcontext, "index",        -1);

            BrainLayer layer = get_network_layer(network, layer_idx);

            if (layer != NULL)
            {
                BrainNeuron neuron = get_layer_neuron(layer, neuron_idx);

                if (neuron != NULL)
                {
                    initialize_neuron_from_context(neuron, subcontext);
                }
            }
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
                        in,
                        network->_use_dropout,
                        network->_dropout_percent);
    }
}

BrainResult
train(BrainNetwork network,
      const BrainData data,
      const BrainDouble target_error,
      const BrainUint max_iter)
{
    BrainDouble error = target_error + 1.0;

    if ((network != NULL)
    &&  (data != NULL)
    &&  (0 <= target_error)
    &&  (1 <= max_iter))
    {
        BrainUint iteration = 0;

        do
        {
            const BrainUint   index         = get_data_random_subset_index(data);
            const BrainUint   input_length  = get_data_input_length(data);
            const BrainUint   output_length = get_data_output_length(data);
            const BrainSignal input         = get_data_input(data, index);
            const BrainSignal output        = get_data_output(data, index);

            feedforward(network, input_length, input);
            error = backpropagate(network, output_length, output);

            ++iteration;
        } while ((iteration < max_iter) && (error > target_error));
    }

    if (error > target_error)
    {
        return BRAIN_FAILED;
    }

    return BRAIN_SUCCESS;
}
