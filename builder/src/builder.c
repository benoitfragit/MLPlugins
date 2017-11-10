#include "brain_plugin.h"

BrainInt
main(BrainInt argc, BrainChar** argv)
{
    BrainInt ret = EXIT_FAILURE;

    BrainPlugin plugin = new_brain_plugin("install/debug/libbrain-1.0.0/api/mlp.xml");

    if (plugin)
    {
        new_plugin_network(plugin, "/home/benoit/Documents/projets/C/libBrain/plugins/test/src/test_create_network.xml");

        serialize_network_at_index(plugin, "/tmp/serialize.xml", 0);

        delete_brain_plugin(plugin);

        ret = EXIT_SUCCESS;
    }

    return ret;
}
