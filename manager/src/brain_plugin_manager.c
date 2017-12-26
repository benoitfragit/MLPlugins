#include "brain_logging_utils.h"
#include "brain_memory_utils.h"
#include "brain_plugin_manager.h"
#include "brain_plugin.h"

#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>

#define PLUGINS manager->_plugins

typedef struct PluginManager
{
    GList* _plugins;
} PluginManager;

static void
autodetect_plugins(BrainPluginManager manager)
{
    if (BRAIN_ALLOCATED(manager))
    {
        BrainChar* path = getenv("BRAIN_PLUGIN_PATH");

        if (BRAIN_ALLOCATED(path))
        {
            BrainChar* subpath = strtok(path, ":");

            do
            {
                GError* error;
                GDir* dir = g_dir_open(subpath, 0, &error);

                if (BRAIN_ALLOCATED(dir))
                {
                    BrainString filename = NULL;

                    while ((filename = g_dir_read_name(dir)) != NULL)
                    {
                        if (g_file_test(filename, G_FILE_TEST_EXISTS)
                        &&  g_file_test(filename, G_FILE_TEST_IS_REGULAR)
                        &&  !g_file_test(filename, G_FILE_TEST_IS_SYMLINK))
                        {
                            BrainString extension = strrchr(filename, '.');

                            if (extension && !strcmp(extension, ".xml"))
                            {
                                BrainChar plugin_definition_file[PATH_MAX + 1];
                                BrainPlugin plugin = NULL;

                                sprintf(plugin_definition_file, "%s/%s", path, filename);

                                plugin = new_plugin(plugin_definition_file);

                                if (BRAIN_ALLOCATED(plugin))
                                {
                                    PLUGINS = g_list_prepend(PLUGINS, plugin);
                                }
                            }
                        }
                    }

                    g_dir_close(dir);
                }
                else
                {
                    BRAIN_CRITICAL("Unable to open the directory: %s\n", path);
                }
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
    BrainPluginManager manager  = NULL;

    BRAIN_NEW(manager, PluginManager, 1);
    PLUGINS = NULL;

    autodetect_plugins(manager);

    return manager;
}

void
delete_plugin_manager(BrainPluginManager manager)
{
    if (BRAIN_ALLOCATED(manager))
    {
        if (BRAIN_ALLOCATED(PLUGINS))
        {
            g_list_free_full(PLUGINS, (GDestroyNotify)delete_plugin);
            BRAIN_DELETE(PLUGINS);
        }

        BRAIN_DELETE(manager);
    }
}

BrainUint
get_number_of_plugins(const BrainPluginManager manager)
{
    BrainUint ret = 0;

    return ret;
}

BrainPlugin
get_plugin_with_index(const BrainPluginManager manager, const BrainUint index)
{
    BrainPlugin plugin = NULL;

    return plugin;
}
