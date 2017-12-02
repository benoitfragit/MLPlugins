#include "config_all.h"

#include "brain_logging_utils.h"
#include "common_test.h"

#include "brain_network.h"

int
main(int argc, char** argv)
{
    BRAIN_INPUT(main)

    BrainReal in[]       = {0.05, 0.1};
    BrainReal desired[]  = {0.01, 0.99};
    BrainInt ret = EXIT_SUCCESS;
    BrainUint i = 0;
    BrainSignal out = NULL;
    const BrainUint number_of_input = 2;

    // Create a neural network using an XML file
    BrainNetwork network = new_network_from_context(TEST_LEARNING_NETWORK_PATH);
    // Create a second neural network to load desired weight values
    BrainNetwork desired_network = new_network_from_context(TEST_LEARNING_NETWORK_PATH);
    // Configure it using XML file
    configure_network_with_context(network, TEST_LEARNING_SETTINGS_PATH);
    // Initialize it using an XML file
    deserialize_network(network, TEST_LEARNING_INIT_PATH);
    // Initialize with desired weights value
    deserialize_network(desired_network, TEST_LEARNING_DESIRED_PATH);
    // Test the forward propagation
    predict(network, number_of_input, in);
    out = get_network_output(network);
    // Test the backpropagation
    backpropagate(network, 2, desired);
    // Check if the backpropagation is a success
    ret = compare_networks(network, desired_network);

    BRAIN_OUTPUT(main);
    return ret;
}
