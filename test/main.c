#include <stdlib.h>
#include <stdio.h>

#include "brain.h"
#include "brain_data_reader.h"

int
main(int argc, char** argv)
{
    Network* network = NULL;
    Data*    data    = NULL;

    if (argc <= 2)
    {
        BRAIN_LOG("Main", "critical", "<%s, %d>: you should pass the network xml file and the data xml file\n", __FILE__, __LINE__);
        return EXIT_FAILURE;
    }

    Document network_document = open_document(argv[1]);
    Document data_document    = open_document(argv[2]);

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
        delete_network(network);
    }

    if (data)
    {
        delete_data(data);
    }

    return EXIT_SUCCESS;
}
