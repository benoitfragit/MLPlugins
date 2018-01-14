#include "brain_plugin.h"
#include "brain_logging_utils.h"
#include "brain_xml_utils.h"
#include "brain_memory_utils.h"
#include "brain_core_types.h"
#include <dlfcn.h>

#define LOAD_NETWORK        plugin->_load_network
#define CONFIGURE           plugin->_configure
#define SERIALIZE           plugin->_serialize
#define DESERIALIZE         plugin->_deserialize
#define PREDICT             plugin->_predict
#define TRAIN_FROM_FILE     plugin->_train_from_file
#define TRAIN_STEP          plugin->_train_step
#define DELETE_NETWORK      plugin->_delete_network
#define TRAINING_REQUIRED   plugin->_is_training_required
#define COMPUTE_ERROR       plugin->_compute_error
#define LOAD_DATA           plugin->_load_data
#define DELETE_DATA         plugin->_delete_data
#define GET_TEST_LENGTH     plugin->_get_test_length
#define SET_ERROR           plugin->_set_error
#define GET_PROGRESS        plugin->_get_progress
#define HANDLE              plugin->_handle

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
    "compute_network_error",
    "new_data_from_context",
    "delete_data",
    "get_number_of_evaluating_sample",
    "set_network_error",
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
    PLUGIN_API_COMPUTE_ERROR,
    PLUGIN_API_LOAD_DATA,
    PLUGIN_API_DELETE_DATA,
    PLUGIN_API_GET_TEST_LENGTH,
    PLUGIN_API_SET_ERROR,
    PLUGIN_API_GET_PROGRESS,
    PLUGIN_API_NONE,

    PLUGIN_API_FIRST = PLUGIN_API_LOAD_NETWORK,
    PLUGIN_API_LAST  = PLUGIN_API_GET_PROGRESS
} PluginApiEnum;

typedef BrainNetwork (*BrainLoadNetworkPtrFunc)     (BrainString);
typedef void         (*BrainConfigurePtrFunc)       (BrainNetwork,       BrainString);
typedef void         (*BrainSerializePtrFunc)       (const BrainNetwork, BrainString);
typedef void         (*BrainDeserializePtrFunc)     (BrainNetwork,       BrainString);
typedef void         (*BrainTrainFromFilePtrFunc)   (BrainNetwork,       BrainString);
typedef void         (*BrainTrainStepPtrFunc)       (BrainNetwork,       const BrainData);
typedef void         (*BrainPredictPtrFunc)         (BrainNetwork,       const BrainUint, const BrainSignal);
typedef void         (*BrainDeleteNetworkPtrFunc)   (BrainNetwork);
typedef BrainBool    (*BrainTrainingRequiredPtrFunc)(const BrainNetwork);
typedef BrainReal    (*BrainComputeErrorPtrFunc)    (const BrainNetwork, const BrainData, const BrainUint);
typedef BrainData    (*BrainLoadDataPtrFunc)        (BrainString);
typedef void         (*BrainDeleteDataPtrFunc)      (BrainData);
typedef BrainUint    (*BrainGetTestLengthPtrFunc)   (const BrainData);
typedef void         (*BrainSetErrorPtrFunc)        (BrainNetwork, const BrainReal);
typedef BrainReal    (*BrainGetProgressPtrFunc)     (const BrainNetwork);
typedef void* BrainHandle;

typedef struct Plugin
{
    /******************************************************************/
    /**                     INTERNAL PARAMETERS                      **/
    /******************************************************************/
    BrainHandle                  _handle;
    /******************************************************************/
    /**                   FUNCTIONAL PARAMETERS                      **/
    /******************************************************************/
    BrainLoadNetworkPtrFunc      _load_network;
    BrainConfigurePtrFunc        _configure;
    BrainSerializePtrFunc        _serialize;
    BrainDeserializePtrFunc      _deserialize;
    BrainTrainFromFilePtrFunc    _train_from_file;
    BrainTrainStepPtrFunc        _train_step;
    BrainPredictPtrFunc          _predict;
    BrainDeleteNetworkPtrFunc    _delete_network;
    BrainTrainingRequiredPtrFunc _is_training_required;
    BrainComputeErrorPtrFunc     _compute_error;
    BrainLoadDataPtrFunc         _load_data;
    BrainDeleteDataPtrFunc       _delete_data;
    BrainGetTestLengthPtrFunc    _get_test_length;
    BrainSetErrorPtrFunc         _set_error;
    BrainGetProgressPtrFunc      _get_progress;
} Plugin;

static void
load_plugin_function(BrainPlugin plugin, const PluginApiEnum function)
{
    if (BRAIN_ALLOCATED(plugin))
    {
        switch(function)
        {
            case PLUGIN_API_LOAD_NETWORK:
            {
                LOAD_NETWORK = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(LOAD_NETWORK))
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
            case PLUGIN_API_TRAIN_FROM_FILE:
            {
                TRAIN_FROM_FILE = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(TRAIN_FROM_FILE))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_TRAIN_STEP:
            {
                TRAIN_STEP = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(TRAIN_STEP))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_DELETE_NETWORK:
            {
                DELETE_NETWORK = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(DELETE_NETWORK))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_TRAINING_REQUIRED:
            {
                TRAINING_REQUIRED = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(TRAINING_REQUIRED))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_COMPUTE_ERROR:
            {
                COMPUTE_ERROR = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(COMPUTE_ERROR))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_LOAD_DATA:
            {
                LOAD_DATA = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(LOAD_DATA))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_DELETE_DATA:
            {
                DELETE_DATA = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(DELETE_DATA))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_GET_TEST_LENGTH:
            {
                GET_TEST_LENGTH = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(GET_TEST_LENGTH))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_SET_ERROR:
            {
                SET_ERROR = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(SET_ERROR))
                {
                    BRAIN_DEBUG("Function: %s has not been loaded", _plugin_functions[function]);
                }
            }
                break;
            case PLUGIN_API_GET_PROGRESS:
            {
                GET_PROGRESS = dlsym(HANDLE, _plugin_functions[function]);
                if (!BRAIN_ALLOCATED(GET_PROGRESS))
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
        && LOAD_NETWORK
        && CONFIGURE
        && SERIALIZE
        && DESERIALIZE
        && TRAIN_FROM_FILE
        && TRAIN_STEP
        && PREDICT
        && DELETE_NETWORK
        && TRAINING_REQUIRED
        && COMPUTE_ERROR
        && LOAD_DATA
        && DELETE_DATA
        && GET_TEST_LENGTH
        && SET_ERROR
        && GET_PROGRESS;
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
        network = LOAD_NETWORK(filename);
    }

    return network;
}

BrainData
new_plugin_data(const BrainPlugin plugin, BrainString filename)
{
    BrainData data = NULL;

    if (BRAIN_ALLOCATED(plugin))
    {
        data = LOAD_DATA(filename);
    }

    return data;
}

void
delete_plugin_network(const BrainPlugin plugin, BrainNetwork network)
{
    if (BRAIN_ALLOCATED(plugin)
    &&  BRAIN_ALLOCATED(network))
    {
        DELETE_NETWORK(network);
    }
}

void
delete_plugin_data(const BrainPlugin plugin, BrainData data)
{
    if (BRAIN_ALLOCATED(plugin)
    &&  BRAIN_ALLOCATED(data))
    {
        DELETE_DATA(data);
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
train_network_from_file(const BrainPlugin plugin, BrainNetwork network, BrainString datapath)
{
    if (BRAIN_ALLOCATED(plugin)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(datapath))
    {
        TRAIN_FROM_FILE(network, datapath);
    }
}

void
train_network_step(const BrainPlugin plugin, BrainNetwork network, const BrainData data)
{
    if (BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(data)
    &&  BRAIN_ALLOCATED(plugin))
    {
        TRAIN_STEP(network, data);
    }
}

BrainBool
is_training_required(const BrainPlugin plugin, const BrainNetwork network)
{
    BrainBool ret = BRAIN_FALSE;

    if (BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(plugin))
    {
        ret = TRAINING_REQUIRED(network);
    }

    return ret;
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

BrainUint
get_test_length(const BrainPlugin plugin, const BrainData data)
{
    BrainUint ret = 0;

    if (BRAIN_ALLOCATED(plugin)
    &&  BRAIN_ALLOCATED(data))
    {
        ret = GET_TEST_LENGTH(data);
    }

    return ret;
}

BrainReal
compute_error(const BrainPlugin plugin,
              const BrainNetwork network,
              const BrainData data,
              const BrainUint i)
{
    BrainReal ret = 0.;

    if (BRAIN_ALLOCATED(plugin)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(data))
    {
        ret = COMPUTE_ERROR(network, data, i);
    }

    return ret;
}

void
set_error(const BrainPlugin plugin,
          BrainNetwork network,
          const BrainReal err)
{
    if (BRAIN_ALLOCATED(plugin)
    &&  BRAIN_ALLOCATED(network))
    {
        SET_ERROR(network, err);
    }
}

BrainReal
get_training_progress(const BrainPlugin plugin, const BrainNetwork network)
{
    BrainReal ret = 0.;

    if (BRAIN_ALLOCATED(plugin)
    &&  BRAIN_ALLOCATED(network))
    {
        ret = GET_PROGRESS(network);
    }

    return ret;
}
