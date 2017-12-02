#include "brain_network.h"
#include "brain_cost.h"
#include "brain_activation.h"
#include "brain_layer.h"
#include "brain_neuron.h"
#include "brain_random.h"
#include "brain_data.h"
#include "brain_xml_utils.h"
#include "brain_logging_utils.h"
#include "brain_config.h"
#include "brain_probe.h"

#define __BRAIN_VISIBLE__ __attribute__((visibility("default")))

/**
 * \struct Network
 * \brief  Internal model for a BrainNetwork
 *
 * All protected fields for a BrainNetwork
 */
struct Network
{
    /*********************************************************************/
    /**                      STRUCTURAL PARAMETERS                      **/
    /*********************************************************************/
    BrainLayer*   _layers;           /*!< layers                         */
    BrainSignal   _output;           /*!< Output signal of the network   */
    BrainSignal   _input;            /*!< Input signal of the network    */
    BrainUint     _number_of_inputs; /*!< Number of inputs               */
    BrainUint     _number_of_layers; /*!< Number of layers               */
    /*********************************************************************/
    /**                      TRAINING PARAMETERS                        **/
    /*********************************************************************/
    BrainDouble   _max_error;        /*!< Maximum error threshold        */
    BrainUint     _max_iter;         /*!< Maximum iteration              */
    /*********************************************************************/
    /**                      FUNCTIONAL PARAMETERS                      **/
    /*********************************************************************/
    CostPtrFunc   _cost_function;    /*!< Cost function                  */
} Network;

static void
feedforward(BrainNetwork      network,
            const BrainUint   number_of_input,
            const BrainSignal in,
            const BrainBool   use_dropout)
{
    BRAIN_INPUT(feedforward)

    if ((in != NULL) &&
        (network != NULL) &&
        (number_of_input == network->_number_of_inputs))
    {
        BrainUint i = 0;

        /**************************************************************/
        /**           FEED THE NETWORK WITH INPUT VECTOR             **/
        /**************************************************************/
        memcpy(network->_input, in, number_of_input * sizeof(BrainDouble));

        for (i = 0; i < network->_number_of_layers; ++i)
        {
            BrainLayer layer = network->_layers[i];

            /**********************************************************/
            /**                    ACTIVATE ALL LAYERS               **/
            /**********************************************************/
            activate_layer(layer, use_dropout && (i != network->_number_of_layers - 1));
        }
    }

    BRAIN_OUTPUT(feedforward)
}

void __BRAIN_VISIBLE__
configure_network_with_context(BrainNetwork network, BrainString filepath)
{
    BRAIN_INPUT(configure_network_with_context)

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
                const BrainUint number_of_layer = network->_number_of_layers;
                BrainUint i = 0;

                BrainChar* buffer = (BrainChar *)node_get_prop(settings_context, "cost-function");
                BrainCostFunctionType cost_function_type = get_cost_function_type(buffer);
                network->_cost_function = get_cost_function(cost_function_type);

                if (buffer != NULL)
                {
                    free(buffer);
                }

                Context training_context = get_node_with_name_and_index(settings_context,
                                                                        "training",
                                                                        0);
                if (training_context != NULL)
                {
                    network->_max_iter  = node_get_int(training_context, "iterations", 1000);
                    network->_max_error = node_get_double(training_context, "error", 0.001);
                }

                for (i = 0; i < number_of_layer; ++i)
                {
                    BrainLayer layer = network->_layers[i];

                    configure_layer_with_context(layer, settings_context);
                }
            }

            close_document(settings_document);
        }
    }

    BRAIN_OUTPUT(configure_network_with_context)
}

BrainLayer
get_network_layer(const BrainNetwork network, const BrainUint index)
{
    BrainLayer ret = NULL;

    if ((network != NULL)
    &&  (network->_layers != NULL)
    &&  (index < network->_number_of_layers))
    {
        ret = network->_layers[index];
    }

    return ret;
}

void
backpropagate(BrainNetwork network,
              const BrainUint number_of_output,
              const BrainSignal desired)
{
    BRAIN_INPUT(backpropagate)
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
    if ((network                   != NULL) &&
        (network->_layers          != NULL) &&
        (network->_number_of_layers!= 0)    &&
        (desired                   != NULL))
    {
        BrainInt i = 0;
        BrainLayer output_layer = network->_layers[network->_number_of_layers - 1];

        backpropagate_output_layer(output_layer, number_of_output, desired);

        for (i = network->_number_of_layers - 2; i >= 0; --i)
        {
            BrainLayer hidden_layer = network->_layers[i];

            backpropagate_hidden_layer(hidden_layer);
        }
    }

    BRAIN_OUTPUT(backpropagate)
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

void __BRAIN_VISIBLE__
delete_network(BrainNetwork network)
{
    BRAIN_INPUT(delete_network)

    if (network != NULL)
    {
        if ((network->_layers) &&
            (network->_number_of_layers != 0))
        {
            BrainUint i = 0;
            for (i = 0; i < network->_number_of_layers; ++i)
            {
                delete_layer(network->_layers[i]);
            }
        }

        if (network->_input)
        {
           free(network->_input);
        }

        free(network);
    }

    BRAIN_OUTPUT(delete_network)
}

static BrainNetwork
new_network(const BrainUint signal_input_length,
            const BrainUint number_of_layers,
            const BrainUint *neuron_per_layers)
{
    BRAIN_INPUT(new_network)

    // initialize the random number generator
    BRAIN_RANDOM_INITIALIZATION

    BrainNetwork _network = NULL;

    if (neuron_per_layers != NULL)
    {
        BrainUint number_of_inputs = signal_input_length;
        _network = (BrainNetwork)calloc(1, sizeof(Network));

        _network->_number_of_inputs = signal_input_length;
        _network->_input            = (BrainSignal)calloc(signal_input_length, sizeof(BrainDouble));
        _network->_layers           = (BrainLayer *)calloc(number_of_layers, sizeof(BrainLayer));
        _network->_number_of_layers = number_of_layers;
        _network->_max_iter         = 1000;
        _network->_max_error        = 0.0001;
        _network->_cost_function    = get_cost_function(Quadratic);

        /**************************************************************/
        /**                INITIALE THE RANDOM GENERATOR             **/
        /**************************************************************/
        if (0 < number_of_layers)
        {
            BrainUint index = 0;

            for (index = 0;
                 index < number_of_layers;
               ++index)
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
predict(BrainNetwork      network,
        const BrainUint   number_of_input,
        const BrainSignal in)
{
    BRAIN_INPUT(predict)

    feedforward(network, number_of_input, in, BRAIN_FALSE);

    BRAIN_INPUT(predict)
}

static BrainBool
isNetworkTrainingRequired(BrainNetwork network, const BrainData data)
{
    /********************************************************/
    /**       Check if we need to train this network       **/
    /********************************************************/
    BrainBool ret = BRAIN_FALSE;

    if ((network != NULL) &&
        (data    != NULL))
    {
        const BrainDouble target_error  = network->_max_error;

        const BrainUint input_length  = get_input_signal_length(data);
        const BrainUint output_length = get_output_signal_length(data);

        const BrainUint number_of_evaluating_sample = get_number_of_evaluating_sample(data);

        BrainSignal input = NULL;
        BrainSignal target = NULL;
        BrainSignal output = NULL;

        BrainDouble error = 0.0;
        BrainUint   i = 0;
        BrainUint   j = 0;

        CostPtrFunc cost_function = network->_cost_function;

        for (i = 0; i < number_of_evaluating_sample; ++i)
        {
            input  = get_evaluating_input_signal(data, i);
            target = get_evaluating_output_signal(data, i);

            // only propagate the signal threw all layers
            predict(network, input_length, input);

            // grab the network output and compute the error
            // between the target and the real output
            output = get_network_output(network);

            for (j = 0; j< output_length; ++j)
            {
                error += cost_function(target[j], output[j]);
            }
        }

        error /= number_of_evaluating_sample;

        if (target_error < error)
        {
            ret = BRAIN_TRUE;
        }
    }

    return ret;
}

void __BRAIN_VISIBLE__
train_network(BrainNetwork network, BrainString repository_path, BrainString tokenizer)
{
    BRAIN_INPUT(train_network)
    /********************************************************/
    /**   Train the neural network using the training set  **/
    /********************************************************/
    if (network != NULL)
    {
        const BrainUint max_iteration = network->_max_iter;
        const BrainUint input_length  = network->_number_of_inputs;
        const BrainUint output_length = get_layer_number_of_neuron(network->_layers[network->_number_of_layers - 1]);

        BrainSignal input = NULL;
        BrainSignal target = NULL;

        BrainUint   iteration = 0;
        BrainUint   i = 0;

        BrainData   data = NULL;

        if (repository_path
        &&  tokenizer)
        {
            data = new_data(repository_path, tokenizer, input_length, output_length, BRAIN_TRUE);
        }

        if (data)
        {
            const BrainUint number_of_training_sample = get_number_of_training_sample(data);

            /**************************************************************/
            /**              TRAIN OVER ALL TRAINING EXAMPLES            **/
            /**************************************************************/
            while ((iteration < max_iteration)
            &&     isNetworkTrainingRequired(network, data))
            {
                for (i = 0; i < number_of_training_sample; ++i)
                {
                    input = get_training_input_signal(data, i);
                    target = get_training_output_signal(data, i);
                    /******************************************************/
                    /**         FORWARD PROPAGATION OF THE SIGNAL        **/
                    /******************************************************/
                    feedforward(network, input_length, input, BRAIN_TRUE);
                    /******************************************************/
                    /**     BACKPROPAGATION USING THE TARGET SIGNAL      **/
                    /******************************************************/
                    backpropagate(network, output_length, target);
                }

                ++iteration;
            }
        }
    }

    BRAIN_OUTPUT(train_network)
}

void __BRAIN_VISIBLE__
deserialize_network(BrainNetwork network, BrainString filepath)
{
    BRAIN_INPUT(deserialize_network)

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
                const BrainUint number_of_serialized_layer = get_number_of_node_with_name(context, "layer");
                const BrainUint number_of_layer = network->_number_of_layers;

                if (number_of_layer == number_of_serialized_layer)
                {
                    BrainUint i = 0;

                    for (i = 0; i < number_of_layer; ++i)
                    {
                        BrainLayer layer = network->_layers[i];
                        Context layer_context = get_node_with_name_and_index(context, "layer", i);

                        deserialize_layer(layer, layer_context);
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
serialize_network(const BrainNetwork network, BrainString filepath)
{
    BRAIN_INPUT(serialize_network)

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
                    const BrainUint number_of_layer = network->_number_of_layers;
                    BrainUint i = 0;

                    for (i = 0; i < number_of_layer;++i)
                    {
                        BrainLayer layer = network->_layers[i];

                        serialize_layer(layer, writer);
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

BrainNetwork __BRAIN_VISIBLE__
new_network_from_context(BrainString filepath)
{
    BRAIN_INPUT(new_network_from_context)

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

    BRAIN_OUTPUT(new_network_from_context)

    return network;
}
