#include "brain_logging_utils.h"
#include "brain_plugin_manager.h"
#include "brain_plugin.h"

#include <limits.h>
#include <dirent.h>
#include <string.h>

typedef struct PluginManager
{
    BrainPlugin* _plugins;
    BrainBool*   _activations;
    BrainUint    _number_of_plugins;
} PluginManager;

static void
autodetect_plugins(BrainPluginManager manager)
{
    if (manager)
    {
        BrainChar* path = getenv("BRAIN_PLUGIN_PATH");

        if (path)
        {
            BrainChar* subpath = strtok(path, ":");

            do
            {
                DIR* dir = opendir(subpath);

                if (dir)
                {
                    struct dirent* content = NULL;

                    while ((content = readdir(dir)) != NULL)
                    {
                        if ((content->d_type != DT_DIR) &&
                            (content->d_type == DT_REG))
                        {
                            BrainString extension = strrchr(content->d_name, '.');

                            if (extension && !strcmp(extension, ".xml"))
                            {
                                BrainChar plugin_definition_file[PATH_MAX + 1];
                                BrainPlugin plugin = NULL;

                                sprintf(plugin_definition_file, "%s/%s", path, content->d_name);

                                plugin = new_plugin(plugin_definition_file);

                                if (plugin)
                                {
                                    ++(manager->_number_of_plugins);

                                    manager->_plugins = (BrainPlugin*)realloc(manager->_plugins, manager->_number_of_plugins*sizeof(BrainPlugin));
                                    manager->_activations = (BrainBool*)realloc(manager->_activations, manager->_number_of_plugins*sizeof(BrainBool));

                                    manager->_plugins[manager->_number_of_plugins - 1] = plugin;
                                    manager->_activations[manager->_number_of_plugins - 1] = BRAIN_FALSE;
                                }
                            }
                        }
                    }
                }
                else
                {
                    BRAIN_CRITICAL("Unable to open the directory: %s\n", path);
                }

                closedir(dir);

            } while ((subpath = strtok(NULL, ":")) != NULL);
        }
        else
        {
            BRAIN_CRITICAL("BRAIN_PLUGIN_PATH is empty. Plugin autoloading not available\n");
        }
    }
}

BrainPluginManager
new_plugin_manager()
{
    BrainPluginManager manager  = (BrainPluginManager)calloc(1, sizeof(PluginManager));
    manager->_number_of_plugins = 0;
    manager->_plugins           = NULL;
    manager->_activations       = NULL;

    autodetect_plugins(manager);

    return manager;
}

void
delete_plugin_manager(BrainPluginManager manager)
{
    if (manager)
    {
        if (manager->_plugins)
        {
            BrainUint index = 0;

            for (index = 0; index < manager->_number_of_plugins; ++index)
            {
                BrainPlugin plugin = get_plugin_with_index(manager, index);

                delete_plugin(plugin);
            }

            free(manager->_plugins);
        }

        if (manager->_activations)
        {
            free(manager->_activations);
        }
    }
}

BrainUint
get_number_of_plugins(const BrainPluginManager manager)
{
    BrainUint ret = 0;

    if (manager)
    {
        ret = manager->_number_of_plugins;
    }

    return ret;
}

BrainPlugin
get_plugin_with_index(const BrainPluginManager manager, const BrainUint index)
{
    BrainPlugin plugin = NULL;

    if (manager &&
        (index < manager->_number_of_plugins))
    {
        plugin = manager->_plugins[index];
    }

    return plugin;
}

BrainBool
is_plugin_activated(const BrainPluginManager manager, const BrainUint index)
{
    BrainBool ret = BRAIN_FALSE;

    if (manager &&
       (index < manager->_number_of_plugins))
    {
        ret = manager->_activations[index];
    }

    return ret;
}

void
set_plugin_activated(const BrainPluginManager manager, const BrainPlugin plugin, const BrainBool activation)
{
    if (manager && plugin)
    {
        const BrainUint number_of_plugins = manager->_number_of_plugins;
        BrainUint i = 0;

        for (i = 0; i < number_of_plugins; ++i)
        {
            if (manager->_plugins[i] == plugin)
            {
                manager->_activations[i] = activation;
                break;
            }
        }
    }
}
