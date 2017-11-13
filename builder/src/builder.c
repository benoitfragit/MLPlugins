#include "brain_plugin.h"
#include "brain_logging_utils.h"

BrainInt
main(BrainInt argc, BrainChar** argv)
{
    BrainInt ret = EXIT_FAILURE;

    BrainPlugin plugin = new_brain_plugin("install/debug/libbrain-1.0.0/api/mlp.xml");

    if (plugin)
    {
        BRAIN_INFO("Plugin:\n");
        BRAIN_INFO("Name:\t%s\n", get_plugin_name(plugin));
        BRAIN_INFO("Date:\t%s\n", get_plugin_date(plugin));
        BRAIN_INFO("Author:\t%s\n", get_plugin_author(plugin));
        BRAIN_INFO("Description:\t%s\n", get_plugin_description(plugin));

        new_plugin_network(plugin, "/home/benoit/Documents/projets/C/libBrain/plugins/test/src/test_create_network.xml");

        serialize_network_at_index(plugin, "/tmp/serialize.xml", 0);

        delete_brain_plugin(plugin);

        ret = EXIT_SUCCESS;
    }

    return ret;
}
