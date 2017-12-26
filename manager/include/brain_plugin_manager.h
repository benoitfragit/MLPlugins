#ifndef BRAIN_PLUGIN_MANAGER_H
#define BRAIN_PLUGIN_MANAGER_H

#include "brain_plugin_types.h"

BrainPluginManager new_plugin_manager();
void        delete_plugin_manager   (BrainPluginManager manager);
BrainUint   get_number_of_plugins   (const BrainPluginManager manager);
BrainPlugin get_plugin_with_index   (const BrainPluginManager manager, const BrainUint index);

#endif /* BRAIN_PLUGIN_MANAGER_H */
