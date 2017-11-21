#include "config_all.h"

#include "brain_logging_utils.h"

#include "brain_network.h"

static double in[] = {0.05, 0.1};
static double desired[] = {0.01, 0.99};

BrainInt
main(int argc, char** argv)
{
    BrainInt ret = EXIT_SUCCESS;
    BrainUint i = 0;
    BrainSignal out = NULL;
    const BrainUint number_of_input = 2;
    const BrainSignal input = in;
    const BrainSignal des = desired;

    // Create a neural network using an XML file
    BrainNetwork network = new_network_from_context(TEST_LEARNING_NETWORK_PATH);
    // Configure it using XML file
    configure_network_with_context(network, TEST_LEARNING_SETTINGS_PATH);
    // Initialize it using an XML file
    deserialize_network(network, TEST_LEARNING_INIT_PATH);
    // Test the forward propagation
    predict(network, number_of_input, input);
    out = get_network_output(network);
    // Test the backpropagation
    backpropagate(network, 2, des);

    return ret;
}
