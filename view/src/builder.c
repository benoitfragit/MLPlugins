#include "brain_plugin_manager.h"
#include "brain_plugin.h"
#include "brain_logging_utils.h"

BrainInt
main(BrainInt argc, BrainChar** argv)
{
    BrainInt ret = EXIT_FAILURE;

    BrainPluginManager plugin_manager = new_plugin_manager();

    delete_plugin_manager(plugin_manager);

    return ret;
}
