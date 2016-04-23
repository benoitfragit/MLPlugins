#include <stdlib.h>
#include <stdio.h>

#include "brain.h"

int
main(int argc, char** argv)
{
    Document doc = open_document("/home/benoit/Documents/projets/C/libBrain/test/mapping.xml");

    if (doc)
    {
        Network* network = new_network_from_context(get_root_node(doc));

        if (network)
        {
            delete_network(network);
        }

        close_document(doc);
    }

    return EXIT_SUCCESS;
}
