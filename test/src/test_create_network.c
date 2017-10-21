#include "config_all.h"
#include "brain_types.h"
#include "brain_network.h"
#include "brain_layer.h"
#include "brain_neuron.h"
#include "brain_logging_utils.h"

static const int NUMBER_OF_LAYERS    = 3;
static const int NUMBER_OF_NEURONS[] = {3, 6, 2};
static const int NUMBER_OF_INPUTS[]  = {5, 3, 6};

BrainInt
main(int argc, char** argv)
{
    BrainInt result = EXIT_FAILURE;

    const BrainNetwork network = new_network_from_context(TEST_CREATE_NETWORK);

    if (network != NULL)
    {
        BrainLayer layer = NULL;
        BrainUint number_of_layers = 0;

        do
        {
            layer = get_network_layer(network, number_of_layers);

            if (layer)
            {
                BrainUint number_of_neurons = get_layer_number_of_neuron(layer);

                if (number_of_neurons == NUMBER_OF_NEURONS[number_of_layers])
                {
                    BrainUint index = 0;

                    for (index = 0; index < number_of_neurons; ++index)
                    {
                        const BrainNeuron neuron = get_layer_neuron(layer, index);

                        if (neuron != NULL)
                        {
                            const BrainUint number_of_inputs = get_neuron_number_of_input(neuron);

                            if (number_of_inputs != NUMBER_OF_INPUTS[number_of_layers])
                            {
                                BRAIN_CRITICAL("%s", "Number of inputs is not valid");
                                break;
                            }
                        }
                        else
                        {
                            BRAIN_CRITICAL("%s", "Neuron is not valid");
                            break;
                        }
                    }

                    ++ number_of_layers;
                }
                else
                {
                    BRAIN_CRITICAL("%s", "Number of neurons is not valid");
                }
            }
        } while (layer != NULL);

        if (number_of_layers == NUMBER_OF_LAYERS)
        {
            result = EXIT_SUCCESS;
        }

        delete_network(network);
    }

    return result;
}
