#include "brain_network.h"
#include "brain_cost.h"
#include "brain_activation.h"
#include "brain_layer.h"
#include "brain_neuron.h"
#include "brain_weight.h"
#include "brain_random.h"
#include "brain_data.h"
#include "brain_xml_utils.h"
#include "brain_logging_utils.h"
#include "brain_config.h"

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

static BrainString _learning_names[] =
{
    "BackPropagation",
    "Resilient"
};

static BrainLearningType
get_learning_type(BrainString learning_name)
{
    BrainUint i = 0;

    for (i = First_Learning; i <= Last_Learning; ++i)
    {
        if (i != First_Learning
        && !strcmp(_learning_names[i - First_Learning - 1], learning_name))
        {
            return i;
        }
    }

    return Invalid_Learning;
}

static void
feedforward(BrainNetwork      network,
            const BrainUint   number_of_input,
            const BrainSignal in,
            const BrainBool   use_dropout)
{
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
}

static void
set_network_parameters( BrainNetwork network,
                         const BrainUint             iterations,
                         const BrainDouble           error,
                         const BrainActivationType   activation_type,
                         const BrainCostFunctionType costfunction_type,
                         const BrainBool             use_dropout,
                         const BrainDouble           dropout_factor,
                         const BrainLearningType     learning_type,
                         const BrainDouble           backpropagation_learning_rate,
                         const BrainDouble           resilient_delta_min,
                         const BrainDouble           resilient_delta_max,
                         const BrainDouble           resilient_eta_positive,
                         const BrainDouble           resilient_eta_negative)
{
    if (network)
    {
        const BrainUint number_of_layers = network->_number_of_layers;
        BrainUint i = 0;

        network->_max_iter  = iterations;
        network->_max_error = error;
        network->_cost_function = get_cost_function(costfunction_type);

        for (i = 0; i < number_of_layers; ++i)
        {
            BrainLayer layer = network->_layers[i];

            set_layer_parameters(layer,
                                 activation_type,
                                 costfunction_type,
                                 use_dropout,
                                 dropout_factor,
                                 learning_type,
                                 backpropagation_learning_rate,
                                 resilient_delta_min,
                                 resilient_delta_max,
                                 resilient_eta_positive,
                                 resilient_eta_negative);
        }
    }
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

static void
backpropagate(BrainNetwork network,
              const BrainUint number_of_output,
              const BrainSignal desired)
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
        if ((network->_layers) &&
            (network->_number_of_layers != 0))
        {
            BrainUint i = 0;
            for (i = 0; i < network->_number_of_layers; ++i)
            {
                BrainLayer layer = network->_layers[i];

                delete_layer(layer);
            }
        }

        if (network->_input)
        {
           free(network->_input);
        }

        free(network);
    }
}

BrainNetwork
new_network(const BrainUint signal_input_length,
            const BrainUint number_of_layers,
            const BrainUint *neuron_per_layers)
{
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
        initialize_random_generator();

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

    return _network;
}

void
predict(BrainNetwork      network,
        const BrainUint   number_of_input,
        const BrainSignal in)
{
    feedforward(network, number_of_input, in, BRAIN_FALSE);
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

void
train_network(BrainNetwork network, BrainString repository_path, BrainString tokenizer)
{
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
}

void
deserialize_network(BrainNetwork network, BrainString filepath)
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
serialize_network(const BrainNetwork network, BrainString filepath)
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
