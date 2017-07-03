#include <math.h>
#include "config_all.h"
#include "brain_builder.h"
#include "brain_network.h"
#include "brain_layer.h"
#include "brain_neuron.h"
#include "brain_weight.h"
#include "brain_logging_utils.h"

static const BrainDouble _epsilon_double_comparaison = 0.000001;

int
main(int argc, char** argv)
{
    BrainInt result = EXIT_SUCCESS;

    const BrainNetwork original_network = new_network_from_context(TEST_CREATE_NETWORK);

    if (original_network != NULL)
    {
        BrainNetwork loaded_network = new_network_from_context(TEST_CREATE_NETWORK);
        BrainLayer   original_layer = NULL;
        BrainLayer   loaded_layer   = NULL;

        // first serialize this network
        serialize(original_network, TEST_SERIALIZE_NETWORK_PATH);

        // then deserialize it
        deserialize(loaded_network, TEST_SERIALIZE_NETWORK_PATH);

        // finally compare it with the original one
        original_layer = get_network_input_layer(original_network);
        loaded_layer   = get_network_input_layer(loaded_network);

        while (original_layer != NULL || loaded_layer != NULL)
        {
            BrainUint original_number_of_neuron = 0;
            BrainUint loaded_number_of_neuron = 0;
            BrainUint neuron_index = 0;

            if ((original_layer != NULL && loaded_layer == NULL) ||
                (original_layer == NULL && loaded_layer != NULL))
            {
                result = EXIT_FAILURE;
                BRAIN_CRITICAL("%s", "The number of layer is not equal");
                break;
            }

            // compare each neuron
            original_number_of_neuron = get_layer_number_of_neuron(original_layer);
            loaded_number_of_neuron   = get_layer_number_of_neuron(loaded_layer);

            if (original_number_of_neuron != loaded_number_of_neuron)
            {
                result = EXIT_FAILURE;
                BRAIN_CRITICAL("%s", "The number of neuron is not equal");
                break;
            }

            for (  neuron_index = 0;
                   neuron_index < original_number_of_neuron;
                 ++neuron_index)
            {
                const BrainNeuron original_neuron = get_layer_neuron(original_layer, neuron_index);
                const BrainNeuron loaded_neuron   = get_layer_neuron(loaded_layer, neuron_index);

                const BrainUint original_neuron_input = get_neuron_number_of_input(original_neuron);
                const BrainUint loaded_neuron_input   = get_neuron_number_of_input(loaded_neuron);

                const BrainWeight original_neuron_bias = get_neuron_bias(original_neuron);
                const BrainWeight loaded_neuron_bias   = get_neuron_bias(loaded_neuron);

                const BrainDouble original_bias_value = get_weight_value(original_neuron_bias);
                const BrainDouble loaded_bias_value   = get_weight_value(loaded_neuron_bias);

                BrainUint input_index = 0;

                if (original_neuron_input != loaded_neuron_input)
                {
                    result = EXIT_FAILURE;
                    BRAIN_CRITICAL("%s", "The number of input is not equal");
                    break;
                }

                if (_epsilon_double_comparaison < fabs(original_bias_value - loaded_bias_value))
                {
                    result = EXIT_FAILURE;
                    BRAIN_CRITICAL("%s", "bias are not equal");
                    break;
                }

                for (input_index = 0; input_index < original_neuron_input; ++input_index)
                {
                    const BrainWeight original_weight = get_neuron_weight(original_neuron, input_index);
                    const BrainWeight loaded_weight.  = get_neuron_weight(loaded_neuron, input_index);

                    const BrainDouble original_value = get_weight_value(original_weight);
                    const BrainDouvle loaded_value   = get_weight_value(loaded_weight);

                    if (_epsilon_double_comparaison < fabs(original_value - loaded_value))
                    {
                        result = EXIT_FAILURE;
                        BRAIN_CRITICAL("%s", "Weights are not equals");
                        break;
                    }
                }

                if (result == EXIT_FAILURE)
                {
                    break;
                }
            }

            if (result == EXIT_FAILURE)
            {
                break;
            }

            loaded_layer   = get_layer_next_layer(loaded_layer);
            original_layer = get_layer_next_layer(original_layer);
        }

        // free all memory
        delete_network(loaded_network);
        delete_network(original_network);
    }

    return result;
}
