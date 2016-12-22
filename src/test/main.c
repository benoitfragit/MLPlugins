#include <stdlib.h>
#include <stdio.h>

#include "brain_builder.h"

int
main(int argc, char** argv)
{
    if (3 <= argc)
    {
        BrainNetwork network = new_network_from_context(argv[1]);
        BrainData       data = new_data_from_context(argv[2]);

        if (network)
        {
            if (argc >= 4)
                initialize_network_from_context(network, argv[3]);

            dump_network(network, "test.xml");

            delete_network(network);
        }

        if (data)
        {
            delete_data(data);
        }
    }
    else
    {
        BRAIN_CRITICAL("Main", "critical", "<%s, %d>: you should pass the network xml file and the data xml file\n", __FILE__, __LINE__);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
