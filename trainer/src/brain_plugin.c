#include "brain_plugin.h"
#include "brain_logging_utils.h"
#include "brain_xml_utils.h"
#include "brain_memory_utils.h"
#include "brain_core_types.h"
#include <dlfcn.h>

#define ACCESS_PRIVATE_MEMBERS(plugin) plugin->priv = G_TYPE_INSTANCE_GET_PRIVATE (plugin, TYPE_BRAIN_PLUGIN, BrainPluginPrivate);\
                                        BrainPluginPrivate *priv = plugin->priv;

static BrainChar* _plugin_functions[] =
{
    "new_network_from_context",
    "configure_network_with_context",
    "serialize_network",
    "deserialize_network",
    "train_network_from_file",
    "train_network_iteration",
    "predict",
    "delete_network",
    "is_network_training_required",
    "new_data_from_context",
    "delete_data",
    "get_network_training_progress"
};

typedef enum PluginApiEnum
{
    PLUGIN_API_LOAD_NETWORK,
    PLUGIN_API_CONFIGURE,
    PLUGIN_API_SERIALIZE,
    PLUGIN_API_DESERIALIZE,
    PLUGIN_API_TRAIN_FROM_FILE,
    PLUGIN_API_TRAIN_STEP,
    PLUGIN_API_PREDICT,
    PLUGIN_API_DELETE_NETWORK,
    PLUGIN_API_TRAINING_REQUIRED,
    PLUGIN_API_LOAD_DATA,
    PLUGIN_API_DELETE_DATA,
    PLUGIN_API_GET_PROGRESS,
    PLUGIN_API_NONE,

    PLUGIN_API_FIRST = PLUGIN_API_LOAD_NETWORK,
    PLUGIN_API_LAST  = PLUGIN_API_GET_PROGRESS
} PluginApiEnum;


typedef void* BrainHandle;

typedef struct _BrainPluginPrivate
{
    BrainHandle _handle;
    BrainBool _loaded;
} _BrainPluginPrivate;

G_DEFINE_TYPE (BrainPlugin, brain_plugin, G_TYPE_OBJECT);

static void
brain_plugin_dispose(GObject *object)
{
    G_OBJECT_CLASS (brain_plugin_parent_class)->dispose (object);
}


static void
brain_plugin_finalize(GObject *object)
{
    BrainPlugin *plugin = BRAIN_PLUGIN(object);

    ACCESS_PRIVATE_MEMBERS(plugin)

    //unref all objects
    if (BRAIN_ALLOCATED(priv))
    {
        // Closing the handle
        if (BRAIN_ALLOCATED(priv->_handle))
        {
            dlclose(priv->_handle);
        }

        G_OBJECT_CLASS (brain_plugin_parent_class)->finalize (object);
    }
}

static void
brain_plugin_set_property (GObject      *gobject,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    BrainPlugin* plugin = BRAIN_PLUGIN(gobject);

    if (BRAIN_ALLOCATED(plugin))
    {
        ACCESS_PRIVATE_MEMBERS(plugin);

        if (BRAIN_ALLOCATED(priv))
        {
            switch (prop_id)
            {
                default:
                    G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
                    break;
            }
        }
    }
}

static void
brain_plugin_get_property (GObject    *gobject,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
    BrainPlugin* plugin = BRAIN_PLUGIN(gobject);

    if (BRAIN_ALLOCATED(plugin))
    {
        ACCESS_PRIVATE_MEMBERS(plugin);

        if (BRAIN_ALLOCATED(priv))
        {
            switch (prop_id)
            {
                default:
                    G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
                    break;
            }
        }
    }
}

static void
brain_plugin_class_init(BrainPluginClass *klass)
{
    if (BRAIN_ALLOCATED(klass))
    {
        g_type_class_add_private (klass, sizeof (BrainPluginPrivate));
        /******************************************************************/
        /**             PASSING ALL REQUIRED FUNCTION POINTERS           **/
        /******************************************************************/
        GObjectClass *gklass = G_OBJECT_CLASS(klass);
        gklass->get_property = brain_plugin_get_property;
        gklass->set_property = brain_plugin_set_property;
        gklass->dispose      = brain_plugin_dispose;
        gklass->finalize     = brain_plugin_finalize;
    }
}

static void
brain_plugin_init(BrainPlugin* plugin)
{
    if (BRAIN_ALLOCATED(plugin))
    {
        ACCESS_PRIVATE_MEMBERS(plugin)

        if (BRAIN_ALLOCATED(priv))
        {
            priv->_handle                = NULL;
            priv->_loaded                = BRAIN_FALSE;

            plugin->load_network         = NULL;
            plugin->configure            = NULL;
            plugin->serialize            = NULL;
            plugin->deserialize          = NULL;
            plugin->train_from_file      = NULL;
            plugin->train                = NULL;
            plugin->predict              = NULL;
            plugin->delete_network       = NULL;
            plugin->is_training_required = NULL;
            plugin->load_data            = NULL;
            plugin->delete_data          = NULL;
            plugin->get_progress         = NULL;
        }
    }
}

static void
load_plugin_function(BrainPlugin* plugin, const PluginApiEnum function)
{
    ACCESS_PRIVATE_MEMBERS(plugin)

    if (BRAIN_ALLOCATED(priv))
    {
        switch(function)
        {
            case PLUGIN_API_LOAD_NETWORK:
            {
                plugin->load_network = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(plugin->load_network))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_CONFIGURE:
            {
                plugin->configure = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(plugin->configure))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_SERIALIZE:
            {
                plugin->serialize = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(plugin->serialize))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_DESERIALIZE:
            {
                plugin->deserialize = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(plugin->deserialize))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_PREDICT:
            {
                plugin->predict = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(plugin->predict))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_TRAIN_FROM_FILE:
            {
                plugin->train_from_file = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(plugin->train_from_file))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_TRAIN_STEP:
            {
                plugin->train = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(plugin->train))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_DELETE_NETWORK:
            {
                plugin->delete_network = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(plugin->delete_network))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_TRAINING_REQUIRED:
            {
                plugin->is_training_required = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(plugin->is_training_required))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_LOAD_DATA:
            {
                plugin->load_data = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(plugin->load_data))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_DELETE_DATA:
            {
                plugin->delete_data = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(plugin->delete_data))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_GET_PROGRESS:
            {
                plugin->get_progress = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(plugin->get_progress))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            default:
                break;
        }
    }
}

static BrainBool
is_plugin_valid(const BrainPlugin* plugin)
{
    return plugin
            && plugin->load_network
            && plugin->configure
            && plugin->serialize
            && plugin->deserialize
            && plugin->train_from_file
            && plugin->train
            && plugin->predict
            && plugin->delete_network
            && plugin->is_training_required
            && plugin->load_data
            && plugin->delete_data
            && plugin->get_progress;
}

BrainPlugin*
brain_plugin_new(BrainString plugin_name)
{
    BrainPlugin* plugin = NULL;

    if (BRAIN_ALLOCATED(plugin_name))
    {
        BrainChar buf[200];
        sprintf(buf, "lib%s.so", plugin_name);
        BrainHandle* handle = dlopen(buf, RTLD_LAZY);

        if (BRAIN_ALLOCATED(handle))
        {
            BrainUint i = 0;
            plugin = (BrainPlugin*)g_object_new(TYPE_BRAIN_PLUGIN, NULL);

            if (BRAIN_ALLOCATED(plugin))
            {
                ACCESS_PRIVATE_MEMBERS(plugin)

                if (BRAIN_ALLOCATED(priv)
                && !priv->_loaded)
                {
                    for (i = PLUGIN_API_FIRST; i <= PLUGIN_API_LAST; ++i)
                    {
                        load_plugin_function(plugin, i);
                    }
                }

                priv->_handle = handle;
                priv->_loaded  = BRAIN_TRUE;

                if (!is_plugin_valid(plugin))
                {
                    priv->_loaded = BRAIN_FALSE;

                    brain_plugin_finalize(G_OBJECT(plugin));
                    plugin = NULL;
                    BRAIN_CRITICAL("Unable to create a plugin:%s\n", plugin_name);
                }
            }
        }
        else
        {
            BRAIN_CRITICAL("Unable to open plugin handle:%s\n", buf);
        }
    }

    return plugin;
}
