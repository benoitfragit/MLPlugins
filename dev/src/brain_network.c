#include "brain_network.h"
#include "brain_cost.h"
#include "brain_activation.h"
#include "brain_layer.h"
#include "brain_neuron.h"
#include "brain_random.h"
#include "brain_data.h"

/**
 * \struct Network
 * \brief  Internal model for a BrainNetwork
 *
 * All protected fields for a BrainNetwork
 */
struct Network
{
    BrainLayer*   _layers;           /*!< layers                         */
    BrainSignal   _output;           /*!< Output signal of the network   */
    BrainSignal   _input;            /*!< Input signal of the network    */
    BrainUint     _number_of_inputs; /*!< Number of inputs               */
    BrainUint     _number_of_layers; /*!< Number of layers               */

    BrainDouble   _max_error;
    BrainUint     _max_iter;

    CostPtrFunc   _cost_function;
} Network;

static BrainString _learning_names[] =
{
    "BackPropagation",
    "Resilient"
};

BrainLearningType
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

void
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
    if ((network                   != NULL) &&
        (network->_layers          != NULL) &&
        (network->_number_of_layers!= 0)    &&
        (desired                   != NULL))
    {
        int i = 0;
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

        // initialize the random number generator
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

                _network->_layers[index] = new_layer(number_of_neurons,
                                                     number_of_inputs,
                                                     in,
                                                     previous_errors);
            }

            _network->_output = get_layer_output(_network->_layers[number_of_layers -1]);
        }
    }

    return _network;
}

void
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

        memcpy(network->_input, in, number_of_input * sizeof(BrainDouble));

        for (i = 0; i < network->_number_of_layers; ++i)
        {
            BrainLayer layer = network->_layers[i];

            activate_layer(layer, use_dropout && (i != network->_number_of_layers - 1));
        }
    }
}


void
apply_network_correction(BrainNetwork network)
{
    if ((network               != NULL) &&
        (network->_layers      != NULL) &&
        (network->_number_of_layers != 0))
    {
        BrainUint i = 0;

        for (i = 0; i < network->_number_of_layers; ++i)
        {
            BrainLayer layer = network->_layers[i];

            if (layer != NULL)
            {
                apply_layer_correction(layer);
            }
        }
    }
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
            feedforward(network, input_length, input, BRAIN_FALSE);

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
train(BrainNetwork network, const BrainData data)
{
    /********************************************************/
    /**   Train the neural network using the training set  **/
    /********************************************************/
    if ((network != NULL) &&
        (data    != NULL))
    {
        const BrainUint max_iteration = network->_max_iter;
        const BrainUint input_length  = get_input_signal_length(data);
        const BrainUint output_length = get_output_signal_length(data);

        const BrainUint number_of_training_sample = get_number_of_training_sample(data);

        BrainSignal input = NULL;
        BrainSignal target = NULL;

        BrainUint   iteration = 0;
        BrainUint   i = 0;

        while ((iteration < max_iteration)
        &&     isNetworkTrainingRequired(network, data))
        {
            for (i = 0; i < number_of_training_sample; ++i)
            {
                input = get_training_input_signal(data, i);
                target = get_training_output_signal(data, i);

                // feed the network to find the corresponding output
                feedforward(network, input_length, input, BRAIN_TRUE);

                // backpropagate the error and accumulate it
                backpropagate(network, output_length, target);

                // apply network correction
                apply_network_correction(network);
            }

            ++iteration;
        }
    }
}
