#include "config_all.h"
#include "brain_types.h"
#include "brain_builder.h"
#include "brain_network.h"
#include "brain_layer.h"
#include "brain_neuron.h"
#include "brain_logging_utils.h"

static const int NUMBER_OF_LAYERS    = 3;
static const int NUMBER_OF_NEURONS[] = {3, 6, 2};
static const int NUMBER_OF_INPUTS[]  = {5, 3, 6};

int
main(int argc, char** argv)
{
    BrainInt result = EXIT_FAILURE;

    const BrainNetwork network = new_network_from_context(TEST_CREATE_NETWORK);

    if (network != NULL)
    {
        BrainLayer layer = get_network_input_layer(network);
        BrainUint number_of_layers = 0;

        if (layer != NULL)
        {
            do
            {
                BrainUint number_of_neurons = get_layer_number_of_neuron(layer);

                if (number_of_neurons == NUMBER_OF_NEURONS[number_of_layers])
                {
                    BrainUint index = 0;
                    BrainBool valid = BRAIN_TRUE;

                    for (index = 0; index < number_of_neurons; ++index)
                    {
                        const BrainNeuron neuron = get_layer_neuron(layer, index);

                        if (neuron != NULL)
                        {
                            const BrainUint number_of_inputs = get_neuron_number_of_input(neuron);

                            if (number_of_inputs != NUMBER_OF_INPUTS[number_of_layers])
                            {
                                BRAIN_CRITICAL("%s", "Number of inputs is not valid");
                                valid = BRAIN_FALSE;
                                break;
                            }
                        }
                        else
                        {
                            BRAIN_CRITICAL("%s", "Neuron is not valid");
                            valid = BRAIN_FALSE;
                            break;
                        }
                    }

                    if (valid)
                    {
                        ++ number_of_layers;
                    }
                }
                else
                {
                    BRAIN_CRITICAL("%s", "Number of neurons is not valid");
                }

                layer = get_layer_next_layer(layer);
            } while (layer != NULL);

            if (number_of_layers == NUMBER_OF_LAYERS)
            {
                result = EXIT_SUCCESS;
            }
        }

        delete_network(network);
    }

    return result;
}
