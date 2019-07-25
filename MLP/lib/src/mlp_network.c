#include "mlp_network.h"
#include "mlp_layer.h"
#include "mlp_neuron.h"
#include "mlp_config.h"

#include "brain_random_utils.h"
#include "brain_xml_utils.h"
#include "brain_logging_utils.h"
#include "brain_math_utils.h"
#include "brain_memory_utils.h"
#include "brain_function_utils.h"

#include "brain_probe.h"

/**
 * \struct Network
 * \brief  Internal model for a MLPNetwork
 *
 * All protected fields for a MLPNetwork
 */
typedef struct Network
{
    /*********************************************************************/
    /**                      STRUCTURAL PARAMETERS                      **/
    /*********************************************************************/
    MLPLayer*     _layers;           /*!< layers                         */
    BrainSignal   _output;           /*!< Output signal of the network   */
    BrainSignal   _input;            /*!< Input signal of the network    */
    BrainUint     _number_of_inputs; /*!< Number of inputs               */
    BrainUint     _number_of_layers; /*!< Number of layers               */
} Network;

void
feedforward(MLPNetwork      network,
            const BrainUint   number_of_input,
            const BrainSignal in,
            const BrainBool   use_dropout)
{
    BRAIN_INPUT(feedforward)

    if (BRAIN_ALLOCATED(in) &&
        BRAIN_ALLOCATED(network) &&
        (number_of_input == network->_number_of_inputs))
    {
        BrainUint i = 0;
        /**************************************************************/
        /**           FEED THE NETWORK WITH INPUT VECTOR             **/
        /**************************************************************/
        memcpy(network->_input, in, number_of_input*sizeof(BrainReal));
        for (i = 0; i < network->_number_of_layers; ++i)
        {
            /**********************************************************/
            /**                    ACTIVATE ALL LAYERS               **/
            /**********************************************************/
            activate_layer(network->_layers[i], use_dropout && (i != network->_number_of_layers - 1));
        }
    }

    BRAIN_OUTPUT(feedforward)
}

void
update_network(MLPNetwork network, BrainReal learning_rate, BrainReal momentum)
{
    BRAIN_INPUT(update_network)

    if (BRAIN_ALLOCATED(network))
    {
        BrainUint i = 0;

        for (i = 0; i < network->_number_of_layers; ++i)
        {
            /**********************************************************/
            /**                    UPDATE ALL LAYERS               **/
            /**********************************************************/
            update_layer(network->_layers[i], learning_rate, momentum);
        }
    }

    BRAIN_OUTPUT(update_network)
}

MLPLayer
get_network_layer(const MLPNetwork network, const BrainUint index)
{
    MLPLayer ret = NULL;

    if (BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(network->_layers)
    &&  (index < network->_number_of_layers))
    {
        ret = network->_layers[index];
    }

    return ret;
}

void
backpropagate(MLPNetwork network,
              const BrainUint number_of_output,
              const BrainSignal loss)
{
    /******************************************************************/
    /**                      BACKPROP ALGORITHM                      **/
    /**                                                              **/
    /** The goal of this algorithm is to reduce the error between a  **/
    /** the output signal and the targer signal.                     **/
    /**                                                              **/
    /** If we not C(W) = E(w_1,...,w_n) the error between the output **/
    /** signal and the target signal computed using a cost function  **/
    /**                                                              **/
    /** For each weight w_i,we want to find µ_i such that C(W)       **/
    /** decrease to a minimum. The way to do that is to backpropagate**/
    /** the gradient of C(W)                                         **/
    /**                                                              **/
    /** We write:                                                    **/
    /**                                                              **/
    /**                      µ_ji = in_i * $_ji                      **/
    /**                                                              **/
    /** Then the weight wij is adjusted :                            **/
    /**                                                              **/
    /**                      w_ji -= n * µ_ji                        **/
    /**                                                              **/
    /** where n is the learning rate                                 **/
    /******************************************************************/
    BRAIN_INPUT(backpropagate)

    if (BRAIN_ALLOCATED(network) &&
        BRAIN_ALLOCATED(network->_layers) &&
        (0 < network->_number_of_layers)    &&
        BRAIN_ALLOCATED(loss))
    {
        BrainUint i = 0;

        /**************************************************************/
        /**                         BACKPROPAGATE THE LOSS           **/
        /**************************************************************/
        backpropagate_output_layer(network->_layers[network->_number_of_layers - 1], number_of_output, loss);

        for (i = 1; i < network->_number_of_layers; ++i)
        {
            backpropagate_hidden_layer(network->_layers[network->_number_of_layers - i - 1]);
        }
    }

    BRAIN_OUTPUT(backpropagate)
}

BrainSignal
get_network_output(const MLPNetwork network)
{
    if (BRAIN_ALLOCATED(network))
    {
        return network->_output;
    }

    return NULL;
}

void __BRAIN_VISIBLE__
delete_network(MLPNetwork network)
{
    BRAIN_INPUT(delete_network)

    if (BRAIN_ALLOCATED(network))
    {
        if (BRAIN_ALLOCATED(network->_layers) &&
            (network->_number_of_layers != 0))
        {
            BrainUint i = 0;
            for (i = 0; i < network->_number_of_layers; ++i)
            {
                delete_layer(network->_layers[i]);
            }
        }

        BRAIN_DELETE(network->_input);
        BRAIN_DELETE(network);
    }

    BRAIN_OUTPUT(delete_network)
}

static MLPNetwork
new_network(const BrainUint signal_input_length,
            const BrainUint number_of_layers,
            const BrainUint *neuron_per_layers,
            const BrainActivationFunction *activation_functions,
            const BrainActivationFunction *derivative_functions)
{
    BRAIN_INPUT(new_network)

    // initialize the random number generator
    BRAIN_RANDOM_INITIALIZATION

    MLPNetwork _network = NULL;

    if (BRAIN_ALLOCATED(neuron_per_layers)
    &&  BRAIN_ALLOCATED(activation_functions)
    &&  BRAIN_ALLOCATED(derivative_functions))
    {
        BrainUint number_of_inputs = signal_input_length;
        BRAIN_NEW(_network, Network, 1);
        _network->_number_of_inputs = signal_input_length;
        BRAIN_NEW(_network->_input, BrainReal, signal_input_length);
        BRAIN_NEW(_network->_layers, MLPLayer, number_of_layers);
        _network->_number_of_layers = number_of_layers;
        /**************************************************************/
        /**                INITIALE THE RANDOM GENERATOR             **/
        /**************************************************************/
        if (0 < number_of_layers)
        {
            BrainUint index = 0;

            for (index = 0; index < number_of_layers; ++index)
            {
                const BrainUint number_of_neurons = neuron_per_layers[index];

                BrainSignal in = _network->_input;
                BrainSignal previous_errors = NULL;

                if (index != 0)
                {
                    in               = get_layer_output(_network->_layers[index - 1]);
                    previous_errors  = get_layer_errors(_network->_layers[index - 1]);
                    number_of_inputs = neuron_per_layers[index - 1];
                }

                /******************************************************/
                /**                 CREATE A NEW LAYER               **/
                /**                                                  **/
                /** All neurons in this layer are fully connected    **/
                /** to all neurons from the previous layer. Besides  **/
                /** All neurons have also a backprop connection      **/
                /** to automatically backpropagate their error to    **/
                /** their previous layer                             **/
                /**                                                  **/
                /**                   output vector                  **/
                /**               --------------------->             **/
                /** PreviousLayer                        Next Layer  **/
                /**               <---------------------             **/
                /**                    error vector                  **/
                /******************************************************/
                _network->_layers[index] = new_layer(number_of_neurons,
                                                     activation_functions[index],
                                                     derivative_functions[index],
                                                     number_of_inputs,
                                                     in,
                                                     previous_errors);
            }

            /**********************************************************/
            /**          AUTOMATICALLY SET THE OUTPUT VECTOR         **/
            /**********************************************************/
            _network->_output = get_layer_output(_network->_layers[number_of_layers -1]);
        }
    }

    BRAIN_OUTPUT(new_network)

    return _network;
}

void __BRAIN_VISIBLE__
predict(MLPNetwork        network,
        const BrainUint   number_of_input,
        const BrainSignal in)
{
    BRAIN_INPUT(predict)
    feedforward(network, number_of_input, in, BRAIN_FALSE);
    BRAIN_OUTPUT(predict)
}

void __BRAIN_VISIBLE__
deserialize_network(MLPNetwork network, BrainString filepath)
{
    BRAIN_INPUT(deserialize_network)

    if (BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(filepath)
    &&  validate_with_xsd(filepath, INIT_XSD_FILE))
    {
        Document init_document = open_document(filepath);

        if (BRAIN_ALLOCATED(init_document))
        {
            Context context = get_root_node(init_document);

            if (BRAIN_ALLOCATED(context))
            {
                const BrainUint number_of_serialized_layer = get_number_of_node_with_name(context, "layer");
                const BrainUint number_of_layer = network->_number_of_layers;

                if (number_of_layer == number_of_serialized_layer)
                {
                    BrainUint i = 0;

                    for (i = 0; i < number_of_layer; ++i)
                    {
                        Context layer_context = get_node_with_name_and_index(context, "layer", i);

                        deserialize_layer(network->_layers[i], layer_context);
                    }
                }
            }

            close_document(init_document);
        }
    }
    else
    {
        BRAIN_CRITICAL("Unable to deserialize file\n");
    }

    BRAIN_OUTPUT(deserialize_network)
}

void __BRAIN_VISIBLE__
serialize_network(const MLPNetwork network, BrainString filepath)
{
    BRAIN_INPUT(serialize_network)

    if (BRAIN_ALLOCATED(filepath))
    {
        if (BRAIN_ALLOCATED(network))
        {
            Writer writer = create_document(filepath, BRAIN_ENCODING);

            if (BRAIN_ALLOCATED(writer))
            {
                // serialize the network
                if (start_element(writer, "network"))
                {
                    // serialize all layers
                    const BrainUint number_of_layer = network->_number_of_layers;
                    BrainUint i = 0;

                    for (i = 0; i < number_of_layer;++i)
                    {
                        serialize_layer(network->_layers[i], writer);
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

    BRAIN_OUTPUT(serialize_network)
}

MLPNetwork __BRAIN_VISIBLE__
new_network_from_context(BrainString filepath)
{
    BRAIN_INPUT(new_network_from_context)

    MLPNetwork  network  = NULL;

    if (BRAIN_ALLOCATED(filepath) &&
        validate_with_xsd(filepath, NETWORK_XSD_FILE))
    {
        Document network_document = open_document(filepath);

        if (BRAIN_ALLOCATED(network_document))
        {
            Context context = get_root_node(network_document);

            if (BRAIN_ALLOCATED(context) &&
                is_node_with_name(context, "network"))
            {
                const BrainUint  number_of_inputs = node_get_int(context, "inputs", 1);

                Context layers_context = get_node_with_name_and_index(context, "layers", 0);

                if (BRAIN_ALLOCATED(layers_context))
                {
                    const BrainUint  number_of_layers = get_number_of_node_with_name(layers_context, "layer");
                    BrainUint* neuron_per_layers =  NULL;
                    BrainChar* buffer = NULL;
                    BrainActivationFunction* activation_functions = NULL;
                    BrainActivationFunction* derivative_functions = NULL;

                    BRAIN_NEW(neuron_per_layers, BrainUint, number_of_layers);
                    BRAIN_NEW(activation_functions, BrainActivationFunction, number_of_layers);
                    BRAIN_NEW(derivative_functions, BrainActivationFunction, number_of_layers);
                    BrainUint  index = 0;

                    for (index = 0; index < number_of_layers; ++index)
                    {
                        Context subcontext       = get_node_with_name_and_index(layers_context, "layer", index);
                        neuron_per_layers[index] = node_get_int(subcontext, "neurons", 1);
                        buffer                   = (BrainChar *)node_get_prop(subcontext, "activation-function");

                        if (BRAIN_ALLOCATED(buffer))
                        {
                            activation_functions[index] = brain_activation_function(buffer);
                            derivative_functions[index] = brain_derivative_function(buffer);

                            BRAIN_DELETE(buffer);
                        }
                        else
                        {
                            activation_functions[index] = brain_activation_function("Sigmoid");
                            derivative_functions[index] = brain_derivative_function("Sigmoid");
                        }
                    }

                    network = new_network(number_of_inputs,
                                          number_of_layers,
                                          neuron_per_layers,
                                          activation_functions,
                                          derivative_functions);

                    BRAIN_DELETE(neuron_per_layers);
                    BRAIN_DELETE(activation_functions);
                    BRAIN_DELETE(derivative_functions);
                }
            }

            close_document(network_document);
        }
    }

    BRAIN_OUTPUT(new_network_from_context)

    return network;
}
