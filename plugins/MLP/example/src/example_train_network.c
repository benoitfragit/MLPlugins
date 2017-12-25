#include "brain_memory_utils.h"
#include "examples_config.h"
#include "brain_network.h"

BrainUint
main(BrainUint argc, BrainChar** argv)
{
    BrainUint ret = EXIT_FAILURE;

    BrainNetwork network = new_network_from_context(TEST_TRAIN_NETWORK_PATH);

    if (BRAIN_ALLOCATED(network))
    {
        configure_network_with_context(network, TEST_TRAIN_SETTINGS_PATH);
        train_network(network, TEST_DATA_PATH, ",", BRAIN_TRUE, BRAIN_FALSE);
        serialize_network(network, TEST_TRAIN_SERIALIZE_PATH);
        delete_network(network);
        ret = EXIT_SUCCESS;
    }

    return ret;
}
