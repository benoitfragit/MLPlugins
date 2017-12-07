#include "common_test.h"

#include "brain_network.h"
#include "brain_layer.h"
#include "brain_neuron.h"
#include "brain_logging_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static const BrainReal _epsilon_double_comparaison = 1e-4;

BrainInt
compare_networks(const BrainNetwork left, const BrainNetwork right)
{
    BrainInt ret = EXIT_SUCCESS;
    BrainLayer left_layer = NULL;
    BrainLayer right_layer = NULL;
    BrainUint index = 0;

    do
    {
        // finally compare it with the original one
        left_layer  = get_network_layer(left, index);
        right_layer = get_network_layer(right, index);

        BrainUint left_number_of_neuron = 0;
        BrainUint right_number_of_neuron = 0;
        BrainUint neuron_index = 0;

        if ((left_layer != NULL && right_layer == NULL) ||
            (left_layer == NULL && right_layer != NULL))
        {
            ret = EXIT_FAILURE;
            break;
        }

        // compare each neuron
        left_number_of_neuron  = get_layer_number_of_neuron(left_layer);
        right_number_of_neuron = get_layer_number_of_neuron(right_layer);

        if (left_number_of_neuron != right_number_of_neuron)
        {
            BRAIN_CRITICAL("Number of neuron are not equals\n");
            ret = EXIT_FAILURE;
            break;
        }

        for (  neuron_index = 0;
               neuron_index < left_number_of_neuron;
             ++neuron_index)
        {
            const BrainNeuron left_neuron       = get_layer_neuron(left_layer, neuron_index);
            const BrainNeuron right_neuron      = get_layer_neuron(right_layer, neuron_index);

            const BrainUint left_neuron_input   = get_neuron_number_of_input(left_neuron);
            const BrainUint right_neuron_input  = get_neuron_number_of_input(right_neuron);

            const BrainReal left_bias_value   = get_neuron_bias(left_neuron);
            const BrainReal right_bias_value  = get_neuron_bias(right_neuron);

            BrainUint input_index = 0;

            if (left_neuron_input != right_neuron_input)
            {
                BRAIN_CRITICAL("Number of input are not equals\n");
                ret = EXIT_FAILURE;
                break;
            }

            if (_epsilon_double_comparaison < fabs(left_bias_value - right_bias_value))
            {
#if BRAIN_ENABLE_DOUBLE_PRECISION
                BRAIN_CRITICAL("Bias are not equals: (left=%lf, right=%lf)\n", left_bias_value, right_bias_value);
#else
                BRAIN_CRITICAL("Bias are not equals: (left=%f, right=%f)\n", left_bias_value, right_bias_value);
#endif
                ret = EXIT_FAILURE;
                break;
            }

            for (input_index = 0; input_index < left_neuron_input; ++input_index)
            {
                const BrainReal left_value  = get_neuron_weight(left_neuron, input_index);
                const BrainReal right_value = get_neuron_weight(right_neuron, input_index);

                if (_epsilon_double_comparaison < fabs(left_value - right_value))
                {
#if BRAIN_ENABLE_DOUBLE_PRECISION
                    BRAIN_CRITICAL("Weight are not equals (left:%lf, right:%lf)\n", left_value, right_value);
#else
                    BRAIN_CRITICAL("Weight are not equals (left:%lf, right:%f)\n", left_value, right_value);
#endif
                    ret = EXIT_FAILURE;
                    break;
                }
            }

            if (ret == EXIT_FAILURE)
            {
                break;
            }
        }

        if (ret == EXIT_FAILURE)
        {
            break;
        }

        ++index;
    } while (left_layer != NULL || right_layer != NULL);

    return ret;
}
