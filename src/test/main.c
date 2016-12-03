#include <stdlib.h>
#include <stdio.h>

#include "brain.h"
#include "brain_data_reader.h"

int
main(int argc, char** argv)
{
    Network_t network = NULL;
    Data_t    data    = NULL;
    Document  network_document, data_document, init_document;

    if (argc <= 2)
    {
        BRAIN_CRITICAL("Main", "critical", "<%s, %d>: you should pass the network xml file and the data xml file\n", __FILE__, __LINE__);
        return EXIT_FAILURE;
    }

    if (validate_with_xsd(argv[1], NETWORK_XSD_FILE) && validate_with_xsd(argv[2], DATA_XSD_FILE))
    {
        network_document = open_document(argv[1]);
        data_document    = open_document(argv[2]);

        if (network_document)
        {
            network = new_network_from_context(get_root_node(network_document));
            close_document(network_document);
        }

        if (data_document)
        {
            data = new_data_from_context(get_root_node(data_document));
            close_document(data_document);
        }

        if (network)
        {
            if (argc >= 4 && validate_with_xsd(argv[3], INIT_XSD_FILE))
            {
                init_document = open_document(argv[3]);

                if (init_document)
                {
                    initialize_network_from_context(network, get_root_node(init_document));
                }
            }

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
        BRAIN_CRITICAL("Main", "Unable to validate network or data file");

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
