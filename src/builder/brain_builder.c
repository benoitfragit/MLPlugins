#include "brain_builder.h"
#include "brain_network_builder.h"
#include "brain_layer_builder.h"
#include "brain_neuron_builder.h"
#include "brain_data_reader.h"
#include "brain_costfunction.h"
#include "brain_activation.h"
#include "brain_logging_utils.h"
#include "brain_xml_utils.h"

BrainNetwork
new_network_from_context(Context context)
{
    BrainNetwork network =  NULL;

    if (context && is_node_with_name(context, "network"))
    {
        BrainChar* buffer = NULL;
        BrainUint index   =    0;

        const BrainUint   number_of_layers             = get_number_of_node_with_name(context, "layer");
        const BrainUint   number_of_inputs             = node_get_int   (context, "inputs",            1);
        const BrainBool   use_dropout                  = node_get_int   (context, "use-dropout",       0);
        const BrainDouble dropout_percent              = node_get_double(context, "dropout-percent", 0.5);
        const BrainDouble learning_rate                = node_get_double(context, "learning-rate",   1.0);
        BrainUint*        neuron_per_layers            = (BrainUint *)calloc(number_of_layers, sizeof(BrainUint));

        buffer                                         = (BrainChar *)node_get_prop(context, "cost-function-type");
        const BrainCostFunctionType cost_function_type = get_cost_function_type(buffer);

        if (buffer != NULL)
        {
            free(buffer);
        }

        buffer                                         = (BrainChar *)node_get_prop(context, "activation-type");
        const BrainActivationType activation_type      = get_activation_type(buffer);

        if (buffer != NULL)
        {
            free(buffer);
        }

        for (index = 0; index < number_of_layers; ++index)
        {
            Context subcontext = get_node_with_name_and_index(context, "layer", index);

            neuron_per_layers[index] = node_get_int(subcontext, "neurons", 1);
        }

        network = new_network(number_of_inputs,
                              number_of_layers,
                              use_dropout,
                              dropout_percent,
                              learning_rate,
                              cost_function_type,
                              activation_type,
                              neuron_per_layers);

        if (neuron_per_layers != NULL)
        {
            free(neuron_per_layers);
        }
    }

    return network;
}

BrainData
new_data_from_context(Context context)
{
    if (context && is_node_with_name(context, "data"))
    {
        BrainChar* buffer = NULL;
        BrainChar*   part = NULL;
        BrainUint       i = 0;
        BrainUint       j = 0;

        srand(time(NULL));

        const BrainUint number_of_signals  = get_number_of_node_with_name(context, "signal");
        const BrainUint signal_length      = node_get_int(context, "signal-length", 0);
        const BrainUint observation_length = node_get_int(context, "observation-length", 0);

        BrainData _data = new_data(number_of_signals,
                                   signal_length,
                                   observation_length);

        for (i = 0; i < number_of_signals; ++i)
        {
            Context signal_context      = get_node_with_name_and_index(context, "signal", i);

            if (signal_context)
            {
                BrainSignal input  = (BrainSignal)calloc(signal_length, sizeof(BrainDouble));
                BrainSignal output = (BrainSignal)calloc(observation_length, sizeof(BrainDouble));

                buffer = (BrainChar *)node_get_prop(signal_context, "input");
                part = strtok(buffer, ", ");

                for (j = 0; j < signal_length; ++j)
                {
                    if (part != NULL)
                    {
                        sscanf(part, "%lf", &(input[j]));
                        part = strtok(NULL, ", ");
                    }
                }

                if (buffer)
                    free(buffer);

                buffer = (BrainChar *)node_get_prop(signal_context, "output");
                part = strtok(buffer, ", ");

                for (j = 0; j < observation_length; ++j)
                {
                    if (part != NULL)
                    {
                        sscanf(part, "%lf", &(output[j]));
                        part = strtok(NULL, ", ");
                    }
                }

                if (buffer)
                    free(buffer);

                append_data(_data, i, input, output);
            }
        }

        return _data;
    }

    return NULL;
}

static void
initialize_neuron_from_context(BrainNeuron neuron,
                               Context context)
{
    if (neuron && context)
    {
        const BrainDouble neuron_bias       = node_get_double(context, "bias", 0.0);
        const BrainUint   number_of_weights = get_number_of_node_with_name(context, "weight");
        BrainUint index = 0;

        set_neuron_bias(neuron, neuron_bias);

        for (index = 0; index < number_of_weights; ++index)
        {
            Context subcontext = get_node_with_name_and_index(context, "weight", index);

            const BrainUint   input_idx = node_get_int   (subcontext, "input",  -1);
            const BrainDouble weight    = node_get_double(subcontext, "value",  0.0);

            set_neuron_weight(neuron, input_idx, weight);
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
