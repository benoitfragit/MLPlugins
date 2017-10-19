#include "brain_builder.h"
#include "brain_network.h"
#include "brain_layer.h"
#include "brain_neuron.h"
#include "brain_weight.h"

#include "brain_data.h"

#include "brain_activation.h"
#include "brain_cost.h"

#include "brain_logging_utils.h"
#include "brain_xml_utils.h"
#include "brain_config.h"

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
                const BrainUint  number_of_inputs = node_get_int(context, "inputs", 1);

                Context layers_context = get_node_with_name_and_index(context, "layers", 0);

                if (layers_context != NULL)
                {
                    const BrainUint  number_of_layers = get_number_of_node_with_name(layers_context, "layer");
                    BrainUint* neuron_per_layers = (BrainUint *)calloc(number_of_layers, sizeof(BrainUint));
                    BrainUint  index = 0;

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
configure_network_with_context(BrainNetwork network, BrainString filepath)
{
    if ((network != NULL) &&
        (filepath != NULL) &&
        validate_with_xsd(filepath, SETTINGS_XSD_FILE))
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
                BrainDouble           dropout_ratio      = 1.0;
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
                        use_dropout   = node_get_bool(dropout_context, "activate", BRAIN_FALSE);
                        dropout_ratio = node_get_double(dropout_context, "factor", 1.0);
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

                /**********************************************/
                /**        SET ALL NETWORKS PARAMETERS       **/
                /**********************************************/
                set_network_parameters(network,
                                        max_iter,
                                        error,
                                        activation_type,
                                        cost_function_type,
                                        use_dropout,
                                        dropout_ratio,
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

            const BrainDouble weight    = node_get_content_as_double(subcontext);

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
                const BrainUint number_of_layer = get_number_of_node_with_name(context, "layer");


                if (number_of_layer > 0)
                {
                    BrainUint layer_index = 0;
                    BrainLayer layer = NULL;

                    do
                    {

                        Context layer_context = get_node_with_name_and_index(context, "layer", layer_index);

                        if (layer_context != NULL)
                        {
                            const BrainUint number_of_neuron = get_number_of_node_with_name(layer_context, "neuron");

                            layer = get_network_layer(network, layer_index);

                            if (layer != NULL && number_of_neuron == get_layer_number_of_neuron(layer))
                            {
                                BrainUint neuron_index = 0;

                                for (neuron_index = 0;
                                     neuron_index < number_of_neuron;
                                     ++neuron_index )
                                {
                                    Context neuron_context = get_node_with_name_and_index(layer_context, "neuron", neuron_index);
                                    BrainNeuron neuron = get_layer_neuron(layer, neuron_index);

                                    if (neuron_context != NULL && neuron != NULL)
                                    {
                                        initialize_neuron_from_context(neuron, neuron_context);
                                    }
                                }
                            }
                            else
                            {
                                BRAIN_CRITICAL("Unable to initialize the network");

                                return;
                            }
                        }

                        ++layer_index;
                    } while (layer != NULL && layer_index < number_of_layer);
                }
            }

            close_document(init_document);
        }
    }
    else
    {
        BRAIN_CRITICAL("Unable to deserialize file\n");
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
                    BrainUint layer_index = 0;
                    BrainLayer layer = NULL;

                    do
                    {
                        layer = get_network_layer(network, layer_index);

                        if (layer != NULL)
                        {
                            // serialize all neurons
                            const BrainUint number_of_neurons = get_layer_number_of_neuron(layer);

                            if (start_element(writer, "layer"))
                            {
                                BrainUint index_neuron = 0;

                                for (index_neuron = 0;
                                     index_neuron < number_of_neurons;
                                     ++index_neuron)
                                {
                                    const BrainNeuron neuron = get_layer_neuron(layer, index_neuron);

                                    if (neuron != NULL)
                                    {
                                        const BrainUint number_of_inputs = get_neuron_number_of_input(neuron);

                                        if (start_element(writer, "neuron"))
                                        {
                                            const BrainWeight bias = get_neuron_bias(neuron);
                                            BrainUint index_input = 0;
                                            BrainChar buffer[50];

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

                            ++layer_index;
                        }

                    } while (layer != NULL);

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
