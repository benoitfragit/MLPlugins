#include "brain_builder.h"
#include "brain_network.h"
#include "brain_layer.h"
#include "brain_neuron.h"
#include "brain_weight.h"

#include "brain_data_reader.h"
#include "brain_settings.h"

#include "brain_activation.h"
#include "brain_learning.h"
#include "brain_cost.h"

#include "brain_logging_utils.h"
#include "brain_xml_utils.h"
#include "brain_utils_config.h"
#include "brain_data_reader.h"

BrainNetwork
new_network_from_context(BrainString filepath)
{
    BrainNetwork  network  = NULL;

    if (filepath != NULL && validate_with_xsd(filepath, NETWORK_XSD_FILE))
    {
        Document network_document = open_document(filepath);

        if (network_document != NULL)
        {
            Context context = get_root_node(network_document);

            if (context && is_node_with_name(context, "network"))
            {
                BrainUint        index            = 0;
                const BrainUint  number_of_inputs = node_get_int(context, "inputs", 1);

                Context layers_context = get_node_with_name_and_index(context,
                                                                      "layers",
                                                                      0);

                if (layers_context != NULL)
                {
                    const BrainUint  number_of_layers = get_number_of_node_with_name(layers_context,
                                                                                     "layer");
                    BrainUint*      neuron_per_layers = (BrainUint *)calloc(number_of_layers, sizeof(BrainUint));

                    for (index = 0; index < number_of_layers; ++index)
                    {
                        Context subcontext = get_node_with_name_and_index(layers_context,
                                                                          "layer",
                                                                          index);

                        neuron_per_layers[index] = node_get_int(subcontext, "neurons", 1);
                    }

                    network = new_network(number_of_inputs,
                                          number_of_layers,
                                          neuron_per_layers);

                    if (neuron_per_layers != NULL)
                    {
                        free(neuron_per_layers);
                    }
                }
            }

            close_document(network_document);
        }
    }

    return network;
}

void
new_settings_from_context(BrainString filepath)
{
    if (filepath != NULL && validate_with_xsd(filepath, SETTINGS_XSD_FILE))
    {
        Document settings_document = open_document(filepath);

        if (settings_document != NULL)
        {
            Context settings_context = get_root_node(settings_document);

            if (settings_context && is_node_with_name(settings_context, "settings"))
            {
                BrainUint             max_iter           = 1000;
                BrainDouble           error              = 0.001;
                BrainBool             use_dropout        = BRAIN_FALSE;
                BrainDouble           dropout_percent    = 1.0;
                BrainDouble           delta_min          = 0.000001;
                BrainDouble           delta_max          = 50.0;
                BrainDouble           eta_positive       = 1.2;
                BrainDouble           eta_negative       = 0.95;
                BrainDouble           learning_rate      = 1.12;
                BrainLearningType     learning_type      = BackPropagation;
                BrainActivationType   activation_type    = Sigmoid;
                BrainCostFunctionType cost_function_type = CrossEntropy;
                BrainChar*            buffer             = NULL;

                Context training_context = get_node_with_name_and_index(settings_context,
                                                                        "training",
                                                                        0);

                if (training_context != NULL)
                {
                    Context dropout_context = get_node_with_name_and_index(training_context,
                                                                           "dropout",
                                                                           0);

                    if (dropout_context != NULL)
                    {
                        use_dropout     = node_get_bool(dropout_context, "activate", BRAIN_FALSE);
                        dropout_percent = node_get_double(dropout_context, "factor", 1.0);
                    }

                    max_iter = node_get_int(training_context, "iterations", 1000);
                    error    = node_get_double(training_context, "error", 0.001);

                    buffer = (BrainChar *)node_get_prop(training_context, "learning");
                    learning_type = get_learning_type(buffer);

                    if (buffer != NULL)
                    {
                        free(buffer);
                    }

                    Context method_context = get_node_with_name_and_index(training_context,
                                                                          "method",
                                                                          0);

                    if (method_context != NULL)
                    {
                        switch (learning_type)
                        {
                            case BackPropagation:
                            {
                                Context backprop_context = get_node_with_name_and_index(method_context, "backprop", 0);

                                if (backprop_context != NULL)
                                {
                                    learning_rate = node_get_double(backprop_context, "learning-rate", 1.2);
                                }
                            }
                                break;
                            case Resilient:
                            {
                                Context rprop_context = get_node_with_name_and_index(method_context, "rprop", 0);

                                if (rprop_context != NULL)
                                {
                                    Context eta_context   = get_node_with_name_and_index(rprop_context, "resilient-eta", 0);
                                    Context delta_context = get_node_with_name_and_index(rprop_context, "resilient-delta", 0);

                                    if (eta_context != NULL)
                                    {
                                        eta_positive = node_get_double(eta_context, "positive", 1.25);
                                        eta_negative = node_get_double(eta_context, "negative", 0.95);
                                    }

                                    if (delta_context != NULL)
                                    {
                                        delta_max = node_get_double(delta_context, "max", 50.0);
                                        delta_min = node_get_double(delta_context, "min", 0.000001);
                                    }
                                }
                            }
                                break;
                            default:
                                break;
                        }
                    }
                }

                buffer             = (BrainChar *)node_get_prop(settings_context, "cost-function");
                cost_function_type = get_cost_function_type(buffer);

                if (buffer != NULL)
                {
                    free(buffer);
                }

                buffer             = (BrainChar *)node_get_prop(settings_context, "activation-function");
                activation_type    = get_activation_type(buffer);

                if (buffer != NULL)
                {
                    free(buffer);
                }

                new_settings(max_iter,
                             error,
                             activation_type,
                             cost_function_type,
                             use_dropout,
                             dropout_percent,
                             learning_type,
                             learning_rate,
                             delta_min,
                             delta_max,
                             eta_positive,
                             eta_negative);
            }

            close_document(settings_document);
        }
    }
}


BrainData
new_data_from_context(BrainString filepath)
{
    if (filepath != NULL && validate_with_xsd(filepath, DATA_XSD_FILE))
    {
        Document data_document = open_document(filepath);

        if (data_document != NULL)
        {
            Context context = get_root_node(data_document);

            if (context && is_node_with_name(context, "data"))
            {
                BrainChar* buffer = NULL;
                BrainChar*   part = NULL;
                BrainUint       i = 0;
                BrainUint       j = 0;
                BrainUint       k = 0;

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
                        const BrainUint number_of_chunks = get_number_of_node_with_name(signal_context, "chunk");
                        BrainSignal input  = (BrainSignal)calloc(signal_length * number_of_chunks,      sizeof(BrainDouble));
                        BrainSignal output = (BrainSignal)calloc(observation_length, sizeof(BrainDouble));

                        for (j = 0; j < number_of_chunks; ++j)
                        {
                            Context chunk_context = get_node_with_name_and_index(signal_context, "chunk", j);

                            if (chunk_context != NULL)
                            {
                                buffer = (BrainChar *)node_get_prop(chunk_context, "input");
                                part = strtok(buffer, ", ");

                                for (k = 0; k < signal_length; ++k)
                                {
                                    if (part != NULL)
                                    {
                                        sscanf(part, "%lf", &(input[j * signal_length + k]));
                                        part = strtok(NULL, ", ");
                                    }
                                }
                            }
                        }

                        if (buffer)
                            free(buffer);

                        buffer = (BrainChar *)node_get_prop(signal_context, "output");
                        part = strtok(buffer, ", ");

                        for (k = 0; k < observation_length; ++k)
                        {
                            if (part != NULL)
                            {
                                sscanf(part, "%lf", &(output[k]));
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

            close_document(data_document);
        }
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

            const BrainDouble weight    = node_get_double(subcontext, "value",  0.0);

            set_neuron_weight(neuron, index, weight);
        }
    }
}

void
deserialize(BrainNetwork network,
            BrainString filepath)
{
    if ((network != NULL)
    &&  (filepath != NULL)
    &&  validate_with_xsd(filepath, INIT_XSD_FILE))
    {
        Document init_document = open_document(filepath);

        if (init_document != NULL)
        {
            Context context = get_root_node(init_document);

            if (context != NULL)
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

            close_document(init_document);
        }
    }
}

void
serialize(const BrainNetwork network, BrainString filepath)
{
    if (filepath != NULL)
    {
        if (network != NULL)
        {
            Writer writer = create_document(filepath, BRAIN_ENCODING);

            if (writer != NULL)
            {
                // serialize the network
                if (start_element(writer, "network"))
                {
                    // serialize all layers
                    BrainLayer layer = get_network_input_layer(network);

                    while (layer != NULL)
                    {
                        // serialize all neurons
                        const BrainUint number_of_neurons = get_layer_number_of_neuron(layer);
                        BrainUint index_neuron = 0;

                        if (start_element(writer, "layer"))
                        {
                            for (index_neuron = 0;
                                 index_neuron < number_of_neurons;
                                 ++index_neuron)
                            {
                                const BrainNeuron neuron = get_layer_neuron(layer, index_neuron);

                                if (neuron != NULL)
                                {
                                    const BrainUint number_of_inputs = get_neuron_number_of_input(neuron);
                                    BrainUint index_input = 0;
                                    BrainChar buffer[50];

                                    if (start_element(writer, "neuron"))
                                    {
                                        const BrainWeight bias = get_neuron_bias(neuron);

                                        if (bias != NULL)
                                        {
                                            sprintf(buffer, "%lf", get_weight_value(bias));

                                            add_attribute(writer, "bias", buffer);
                                        }

                                        for (index_input = 0;
                                             index_input < number_of_inputs;
                                             ++index_input)
                                        {
                                            const BrainWeight weight = get_neuron_weight(neuron, index_input);

                                            if (weight != NULL)
                                            {
                                                sprintf(buffer, "%lf", get_weight_value(weight));

                                                write_element(writer, "weight", buffer);
                                            }
                                        }

                                        stop_element(writer);
                                    }
                                }
                            }

                            stop_element(writer);
                        }

                        layer = get_layer_next_layer(layer);
                    }

                    stop_element(writer);
                }

                close_writer(writer);
            }
            else
            {
                BRAIN_CRITICAL("Unable to create XML writer\n");
            }
        }
        else
        {
            BRAIN_CRITICAL("Network is not valid\n");
        }
    }
    else
    {
        BRAIN_CRITICAL("XML serializing file is not valid\n");
    }
}

BrainBool
train(BrainNetwork network,
      const BrainData data)
{
    if ((network != NULL)
    &&  (data != NULL))
    {
        const BrainUint   max_iter     = get_settings_max_iterations();
        const BrainDouble target_error = get_settings_target_error();
        BrainDouble error = target_error + 1.0;
        BrainUint iteration = 0;

        do
        {
            const BrainUint   index            = get_data_random_subset_index(data);
            const BrainUint   input_length     = get_data_input_length    (data);
            const BrainUint   output_length    = get_data_output_length   (data);
            const BrainUint   number_of_chunks = get_data_number_of_chunks(data, index);
            const BrainSignal output           = get_data_output          (data, index);
            BrainUint         chunk_index      = 0;

            for (chunk_index = 0; chunk_index < number_of_chunks; ++chunk_index)
            {
                const BrainSignal input = get_data_input( data, index, chunk_index);
                feedforward(network, input_length, input);
                error = backpropagate(network, output_length, output);
            }

            ++iteration;
        } while ((iteration < max_iter) && (error > target_error));

        if (error < target_error)
        {
            return BRAIN_TRUE;
        }
    }

    return BRAIN_FALSE;
}
