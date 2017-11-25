#include "config_all.h"
#include "brain_logging_utils.h"
#include "brain_network.h"
#include "common_test.h"


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

        BrainUint    index          = 0;

        // first serialize this network
        serialize_network(original_network, TEST_SERIALIZE_NETWORK_PATH);

        // then deserialize it
        deserialize_network(loaded_network, TEST_SERIALIZE_NETWORK_PATH);

        result = compare_networks(original_network, loaded_network);

        // free all memory
        delete_network(loaded_network);
        delete_network(original_network);
    }

    return result;
}
