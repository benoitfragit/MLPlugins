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

        klass->load_network         = NULL;
        klass->configure            = NULL;
        klass->serialize            = NULL;
        klass->deserialize          = NULL;
        klass->train_from_file      = NULL;
        klass->train                = NULL;
        klass->predict              = NULL;
        klass->delete_network       = NULL;
        klass->is_training_required = NULL;
        klass->load_data            = NULL;
        klass->delete_data          = NULL;
        klass->get_progress         = NULL;

        klass->loaded               = BRAIN_FALSE;
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
            priv->_handle = NULL;
        }
    }
}

static void
load_plugin_function(BrainPlugin* plugin, const PluginApiEnum function)
{
    BrainPluginClass* klass = BRAIN_PLUGIN_CLASS(plugin);

    ACCESS_PRIVATE_MEMBERS(plugin)

    if (BRAIN_ALLOCATED(priv)
    &&  BRAIN_ALLOCATED(klass))
    {
        switch(function)
        {
            case PLUGIN_API_LOAD_NETWORK:
            {
                klass->load_network = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(klass->load_network))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_CONFIGURE:
            {
                klass->configure = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(klass->configure))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_SERIALIZE:
            {
                klass->serialize = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(klass->serialize))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_DESERIALIZE:
            {
                klass->deserialize = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(klass->deserialize))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_PREDICT:
            {
                klass->predict = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(klass->predict))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_TRAIN_FROM_FILE:
            {
                klass->train_from_file = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(klass->train_from_file))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_TRAIN_STEP:
            {
                klass->train = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(klass->train))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_DELETE_NETWORK:
            {
                klass->delete_network = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(klass->delete_network))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_TRAINING_REQUIRED:
            {
                klass->is_training_required = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(klass->is_training_required))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_LOAD_DATA:
            {
                klass->load_data = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(klass->load_data))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_DELETE_DATA:
            {
                klass->delete_data = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(klass->delete_data))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_GET_PROGRESS:
            {
                klass->get_progress = dlsym(priv->_handle, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(klass->get_progress))
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
    BrainPluginClass* klass = BRAIN_PLUGIN_CLASS(plugin);

    return klass
            && klass->load_network
            && klass->configure
            && klass->serialize
            && klass->deserialize
            && klass->train_from_file
            && klass->train
            && klass->predict
            && klass->delete_network
            && klass->is_training_required
            && klass->load_data
            && klass->delete_data
            && klass->get_progress;
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
                BrainPluginClass* klass = BRAIN_PLUGIN_CLASS(plugin);

                if (BRAIN_ALLOCATED(klass)
                && klass->loaded)
                {
                    for (i = PLUGIN_API_FIRST; i <= PLUGIN_API_LAST; ++i)
                    {
                        load_plugin_function(plugin, i);
                    }
                }

                ACCESS_PRIVATE_MEMBERS(plugin);
                priv->_handle = handle;
            }
        }
        else
        {
            BRAIN_CRITICAL("Unable to open plugin handle:%s\n", buf);
        }

        if (!is_plugin_valid(plugin))
        {
            brain_plugin_finalize(G_OBJECT(plugin));
            plugin = NULL;
            BRAIN_CRITICAL("Unable to create a plugin:%s\n", plugin_name);
        }
    }

    return plugin;
}
