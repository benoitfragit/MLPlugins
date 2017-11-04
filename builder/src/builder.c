#include "brain_builder.h"

BrainInt
main(BrainInt argc, BrainChar** argv)
{
    BrainInt ret = EXIT_FAILURE;

    BrainBuilder builder = new_brain_builder("install/debug/libbrain-1.0.0/api/mlp.xml");

    if (builder)
    {
        new_builder_network(builder, "/home/benoit/Documents/projets/C/libBrain/plugins/test/src/test_create_network.xml");

        serialize_network_at_index(builder, "/tmp/serialize.xml", 0);

        delete_brain_builder(builder);
        ret = EXIT_SUCCESS;
    }

    return ret;
}
