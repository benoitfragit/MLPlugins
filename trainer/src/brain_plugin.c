#include "brain_plugin.h"
#include "brain_logging_utils.h"
#include "brain_xml_utils.h"
#include "brain_memory_utils.h"
#include "brain_core_types.h"
#include <dlfcn.h>

#define LOAD        plugin->_load
#define CONFIGURE   plugin->_configure
#define SERIALIZE   plugin->_serialize
#define DESERIALIZE plugin->_deserialize
#define PREDICT     plugin->_predict
#define TRAIN       plugin->_train
#define DELETE      plugin->_delete
#define NETWORKS    plugin->_networks
#define HANDLE      plugin->_handle

static BrainChar* _plugin_functions[] =
{
    "new_network_from_context",
    "configure_network_with_context",
    "serialize_network",
    "deserialize_network",
    "train_network",
    "predict",
    "delete_network"
};

typedef enum PluginApiEnum
{
    PLUGIN_API_LOAD        = 0,
    PLUGIN_API_CONFIGURE   = 1,
    PLUGIN_API_SERIALIZE   = 2,
    PLUGIN_API_DESERIALIZE = 3,
    PLUGIN_API_TRAIN       = 4,
    PLUGIN_API_PREDICT     = 5,
    PLUGIN_API_DELETE      = 6,
    PLUGIN_API_NONE        = 7,

    PLUGIN_API_FIRST = PLUGIN_API_LOAD,
    PLUGIN_API_LAST  = PLUGIN_API_DELETE
} PluginApiEnum;

typedef BrainNetwork (*BrainLoaderPtrFunc)     (BrainString);
typedef void         (*BrainConfigurePtrFunc)  (BrainNetwork,       BrainString);
typedef void         (*BrainSerializePtrFunc)  (const BrainNetwork, BrainString);
typedef void         (*BrainDeserializePtrFunc)(BrainNetwork,       BrainString);
typedef void         (*BrainTrainPtrFunc)      (BrainNetwork,       BrainString);
typedef void         (*BrainPredictPtrFunc)    (BrainNetwork,       const BrainUint, const BrainSignal);
typedef void         (*BrainDeletePtrFunc)     (BrainNetwork);
typedef void* BrainHandle;

typedef struct Plugin
{
    /******************************************************************/
    /**                     INTERNAL PARAMETERS                      **/
    /******************************************************************/
    BrainHandle             _handle;
    /******************************************************************/
    /**                   FUNCTIONAL PARAMETERS                      **/
    /******************************************************************/
    BrainLoaderPtrFunc      _load;
    BrainConfigurePtrFunc   _configure;
    BrainSerializePtrFunc   _serialize;
    BrainDeserializePtrFunc _deserialize;
    BrainTrainPtrFunc       _train;
    BrainPredictPtrFunc     _predict;
    BrainDeletePtrFunc      _delete;
} Plugin;

static void
load_plugin_function(BrainPlugin plugin, const PluginApiEnum function)
{
    if (BRAIN_ALLOCATED(plugin))
    {
        switch(function)
        {
            case PLUGIN_API_LOAD:
            {
                LOAD = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(LOAD))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_CONFIGURE:
            {
                CONFIGURE = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(CONFIGURE))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_SERIALIZE:
            {
                SERIALIZE = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(SERIALIZE))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_DESERIALIZE:
            {
                DESERIALIZE = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(DESERIALIZE))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_PREDICT:
            {
                PREDICT = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(PREDICT))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_TRAIN:
            {
                TRAIN = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(TRAIN))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_DELETE:
            {
                DELETE = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(DELETE))
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
isPluginValid(const BrainPlugin plugin)
{
    return plugin
        && LOAD
        && CONFIGURE
        && SERIALIZE
        && DESERIALIZE
        && TRAIN
        && PREDICT
        && DELETE;
}

BrainPlugin
new_plugin(BrainString plugin_name)
{
    BrainPlugin plugin = NULL;

    if (BRAIN_ALLOCATED(plugin_name))
    {
        BrainChar buf[200];
        sprintf(buf, "lib%s.so", plugin_name);
        BrainHandle* handle = dlopen(buf, RTLD_LAZY);

        if (BRAIN_ALLOCATED(handle))
        {
            BrainUint i = 0;
            BRAIN_NEW(plugin, Plugin, 1);
            HANDLE = handle;
            for (i = PLUGIN_API_FIRST; i <= PLUGIN_API_LAST; ++i)
            {
                load_plugin_function(plugin, i);
            }
        }
        else
        {
            BRAIN_CRITICAL("Unable to open plugin handle:%s\n", buf);
        }

        if (!isPluginValid(plugin))
        {
            delete_plugin(plugin);
            BRAIN_CRITICAL("Unable to create a plugin:%s\n", plugin_name);
        }
    }

    return plugin;
}

void
delete_plugin(BrainPlugin plugin)
{
    if (BRAIN_ALLOCATED(plugin))
    {
        // Closing the handle
        if (BRAIN_ALLOCATED(HANDLE))
        {
            dlclose(HANDLE);
        }
        // finally free the plugin itself
        BRAIN_DELETE(plugin);
    }
}

BrainNetwork
new_plugin_network(const BrainPlugin plugin, BrainString filename)
{
    BrainNetwork network = NULL;

    if (BRAIN_ALLOCATED(plugin))
    {
        network = LOAD(filename);
    }

    return network;
}

void
delete_network(const BrainPlugin plugin, BrainNetwork network)
{

    if (BRAIN_ALLOCATED(plugin)
    &&  BRAIN_ALLOCATED(network))
    {
        DELETE(network);
    }
}

void
configure_network(const BrainPlugin plugin, BrainNetwork network, BrainString filename)
{

    if (BRAIN_ALLOCATED(plugin)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(filename))
    {
        CONFIGURE(network, filename);
    }
}

void
get_network_prediction(const BrainPlugin plugin, BrainNetwork network, const BrainUint length, const BrainSignal signal)
{
    if (BRAIN_ALLOCATED(plugin)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(signal))
    {
        PREDICT(network, length, signal);
    }
}

void
train_network(const BrainPlugin plugin, BrainNetwork network, BrainString datapath)
{
    if (BRAIN_ALLOCATED(plugin)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(datapath))
    {
        TRAIN(network, datapath);
    }
}

void
serialize_network(const BrainPlugin plugin, BrainNetwork network, BrainString filename)
{
    if (BRAIN_ALLOCATED(plugin)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(filename))
    {
        SERIALIZE(network, filename);
    }
}

void
deserialize_network(const BrainPlugin plugin, BrainNetwork network, BrainString filename)
{
    if (BRAIN_ALLOCATED(plugin)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(filename))
    {
        DESERIALIZE(network, filename);
    }
}
