#include "brain_plugin.h"
#include "brain_manager_config.h"
#include "brain_logging_utils.h"
#include "brain_xml_utils.h"
#include "brain_memory_utils.h"
#include "brain_core_types.h"
#include <gmodule.h>
#include <glib.h>

#define LOAD        plugin->_load
#define CONFIGURE   plugin->_configure
#define SERIALIZE   plugin->_serialize
#define DESERIALIZE plugin->_deserialize
#define PREDICT     plugin->_predict
#define TRAIN       plugin->_train
#define DELETE      plugin->_delete
#define NETWORKS    plugin->_networks
#define HANDLE      plugin->_handle
#define NAME        plugin->_name
#define AUTHOR      plugin->_author
#define DESCRIPTION plugin->_description
#define DATE        plugin->_date
#define ACTIVATED   plugin->_activated

static BrainString _plugin_api_keywords[] = {
    "load",
    "configure",
    "serialize",
    "deserialize",
    "train",
    "predict",
    "delete"
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
typedef void         (*BrainTrainPtrFunc)      (BrainNetwork,       BrainString,     BrainString, BrainBool, BrainBool);
typedef void         (*BrainPredictPtrFunc)    (BrainNetwork,       const BrainUint, const BrainSignal);
typedef void         (*BrainDeletePtrFunc)     (BrainNetwork);

typedef struct Plugin
{
    /******************************************************************/
    /**                     METADATAS PARAMETERS                     **/
    /******************************************************************/
    BrainChar*              _name;
    BrainChar*              _author;
    BrainChar*              _description;
    BrainChar*              _date;
    /******************************************************************/
    /**                     INTERNAL PARAMETERS                      **/
    /******************************************************************/
    GModule*                _handle;
    GList*                  _networks;
    BrainBool               _activated;
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

static PluginApiEnum
get_plugin_api_field(BrainString fieldname)
{
    BrainUint i = 0;
    PluginApiEnum field = PLUGIN_API_NONE;

    for (i = PLUGIN_API_FIRST; i <= PLUGIN_API_LAST; ++i)
    {
        if (!strcmp(fieldname, _plugin_api_keywords[i]))
        {
            field = (PluginApiEnum)i;
            break;
        }
    }

    return field;
}

static void
load_plugin_function(BrainPlugin plugin, Context context)
{
    if (BRAIN_ALLOCATED(context))
    {
        BrainChar* buffer = (BrainChar*)node_get_prop(context, "name");

        if (BRAIN_ALLOCATED(buffer))
        {
            const PluginApiEnum field = get_plugin_api_field(buffer);
            BrainChar* symbol = (BrainChar*)node_get_prop(context, "symbol");

            if (BRAIN_ALLOCATED(symbol))
            {
                switch(field)
                {
                    case PLUGIN_API_LOAD:
                        g_module_symbol(HANDLE, symbol, (gpointer)LOAD);
                        break;
                    case PLUGIN_API_CONFIGURE:
                        g_module_symbol(HANDLE, symbol, (gpointer)CONFIGURE);
                        break;
                    case PLUGIN_API_SERIALIZE:
                        g_module_symbol(HANDLE, symbol, (gpointer)SERIALIZE);
                        break;
                    case PLUGIN_API_DESERIALIZE:
                        g_module_symbol(HANDLE, symbol, (gpointer)DESERIALIZE);
                        break;
                    case PLUGIN_API_PREDICT:
                        g_module_symbol(HANDLE, symbol, (gpointer)PREDICT);
                        break;
                    case PLUGIN_API_TRAIN:
                        g_module_symbol(HANDLE, symbol, (gpointer)TRAIN);
                        break;
                    case PLUGIN_API_DELETE:
                        g_module_symbol(HANDLE, symbol, (gpointer)DELETE);
                        break;
                    default:
                        break;
                }

                BRAIN_DELETE(symbol);
            }

            BRAIN_DELETE(buffer);
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
new_plugin(BrainString plugin_definition_file)
{
    BrainPlugin plugin = NULL;

    if (BRAIN_ALLOCATED(plugin_definition_file))
    {
        if (validate_with_xsd(plugin_definition_file, BRAIN_PLUGIN_DEFINITION_XSD))
        {
            Document plugin_document = open_document(plugin_definition_file);

            if (BRAIN_ALLOCATED(plugin_document))
            {
                Context plugin_context = get_root_node(plugin_document);

                if (BRAIN_ALLOCATED(plugin_context) && is_node_with_name(plugin_context, "plugin"))
                {
                    BrainChar* buffer = (BrainChar *)node_get_prop(plugin_context, "name");

                    if (BRAIN_ALLOCATED(buffer))
                    {
                        BrainString module_path = g_module_build_path(".",buffer);
                        GModule* handle = g_module_open(module_path, G_MODULE_BIND_LAZY);

                        if (BRAIN_ALLOCATED(handle))
                        {
                            BRAIN_NEW(plugin, Plugin, 1);

                            HANDLE      = handle;
                            NETWORKS    = NULL;
                            NAME        = NULL;
                            DESCRIPTION = NULL;
                            AUTHOR      = NULL;
                            ACTIVATED   = BRAIN_FALSE;

                            if (BRAIN_ALLOCATED(HANDLE))
                            {
                                const BrainUint number_of_function = get_number_of_node_with_name(plugin_context, "function");
                                BrainUint i = 0;

                                BRAIN_NEW(NAME, BrainChar, strlen(buffer));
                                strcpy(NAME, buffer);
                                BRAIN_DELETE(buffer);

                                buffer = (BrainChar *)node_get_prop(plugin_context, "author");
                                if (BRAIN_ALLOCATED(buffer))
                                {
                                    BRAIN_NEW(AUTHOR, BrainChar, strlen(buffer));
                                    strcpy(AUTHOR, buffer);
                                    BRAIN_DELETE(buffer);
                                }
                                buffer = (BrainChar *)node_get_prop(plugin_context, "description");
                                if (BRAIN_ALLOCATED(buffer))
                                {
                                    BRAIN_NEW(DESCRIPTION, BrainChar, strlen(buffer));
                                    strcpy(DESCRIPTION, buffer);
                                    BRAIN_DELETE(buffer);
                                }
                                buffer = (BrainChar *)node_get_prop(plugin_context, "date");
                                if (BRAIN_ALLOCATED(buffer))
                                {
                                    BRAIN_NEW(DATE, BrainChar, strlen(buffer));
                                    strcpy(DATE, buffer);
                                    BRAIN_DELETE(buffer);
                                }

                                for (i = 0; i < number_of_function; ++i)
                                {
                                    Context subcontext = get_node_with_name_and_index(plugin_context, "function", i);

                                    load_plugin_function(plugin, subcontext);
                                }
                            }
                        }
                        else
                        {
                            BRAIN_CRITICAL("Unable to open the GModule for the plugin:%s\n", buffer);
                            BRAIN_DELETE(buffer);
                        }
                    }
                    else
                    {
                        BRAIN_CRITICAL("Plugin name not loaded\n");
                    }

                    if (isPluginValid(plugin))
                    {
                        ACTIVATED = BRAIN_TRUE;

                        BRAIN_INFO("Plugin:\n");
                        BRAIN_INFO("Name:\t%s\n", get_plugin_name(plugin));
                        BRAIN_INFO("Date:\t%s\n", get_plugin_date(plugin));
                        BRAIN_INFO("Author:\t%s\n", get_plugin_author(plugin));
                        BRAIN_INFO("Description:\t%s\n", get_plugin_description(plugin));
                    }
                    else
                    {
                        delete_plugin(plugin);

                        BRAIN_CRITICAL("Unable to create a plugin\n");
                    }
                }

                // Close the plugin document
                close_document(plugin_document);
            }
        }
        else
        {
            BRAIN_WARNING("Plugin definition file is not valid\n");
        }
    }
    else
    {
        BRAIN_WARNING("Plugin defintion file is NULL\n");
    }

    return plugin;
}

void
delete_plugin(BrainPlugin plugin)
{
    if (BRAIN_ALLOCATED(plugin))
    {
        // Removing all attached networks
        g_list_free_full(NETWORKS, (GDestroyNotify)DELETE);
        BRAIN_DELETE(NETWORKS);
        // Closing the handle
        g_module_close(HANDLE);
        // Cleaning all metadata
        BRAIN_DELETE(NAME);
        BRAIN_DELETE(DATE);
        BRAIN_DELETE(DESCRIPTION);
        BRAIN_DELETE(AUTHOR);
        // finally free the plugin itself
        BRAIN_DELETE(plugin);
    }
}

BrainNetwork
new_plugin_network(const BrainPlugin plugin, BrainString filename)
{
    BrainNetwork network = NULL;

    if (BRAIN_ALLOCATED(plugin)
    && ACTIVATED)
    {
        network = LOAD(filename);

        if (BRAIN_ALLOCATED(network))
        {
            NETWORKS = g_list_prepend(NETWORKS, network);
        }
    }

    return network;
}

BrainBool
delete_network(const BrainPlugin plugin, BrainNetwork network)
{
    BrainBool ret = BRAIN_FALSE;

    if (BRAIN_ALLOCATED(plugin)
    &&  ACTIVATED
    &&  BRAIN_ALLOCATED(network))
    {
        GList* element = g_list_find(NETWORKS, network);
        if (BRAIN_ALLOCATED(element))
        {
            NETWORKS = g_list_remove(NETWORKS, network);
            DELETE(network);
            ret = BRAIN_TRUE;
        }
    }

    return ret;
}

BrainBool
configure_network(const BrainPlugin plugin, const BrainNetwork network, BrainString filename)
{
    BrainBool ret = BRAIN_FALSE;

    if (BRAIN_ALLOCATED(plugin)
    &&  ACTIVATED
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(filename))
    {
        GList* element = g_list_find(NETWORKS, network);
        if (BRAIN_ALLOCATED(element))
        {
            CONFIGURE(element->data, filename);
            ret = BRAIN_TRUE;
        }
    }

    return ret;
}

BrainBool
get_network_prediction(const BrainPlugin plugin, const BrainNetwork network, const BrainUint length, const BrainSignal signal)
{
    BrainBool ret = BRAIN_FALSE;

    if (BRAIN_ALLOCATED(plugin)
    &&  ACTIVATED
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(signal))
    {
        GList* element = g_list_find(NETWORKS, network);
        if (BRAIN_ALLOCATED(element))
        {
            PREDICT(element->data, length, signal);
            ret = BRAIN_TRUE;
        }
    }

    return ret;
}

BrainBool
train_network(const BrainPlugin plugin, const BrainNetwork network, BrainString repository, BrainString tokenizer, const BrainBool labelled, const BrainBool normalized)
{
    BrainBool ret = BRAIN_FALSE;

    if (BRAIN_ALLOCATED(plugin)
    &&  ACTIVATED
    &&  BRAIN_ALLOCATED(network))
    {
        GList* element = g_list_find(NETWORKS, network);
        if (BRAIN_ALLOCATED(element))
        {
            TRAIN(element->data, repository, tokenizer, labelled, normalized);
            ret = BRAIN_TRUE;
        }
    }

    return ret;
}

BrainBool
serialize_network(const BrainPlugin plugin, const BrainNetwork network, BrainString filename)
{
    BrainBool ret = BRAIN_FALSE;

    if (BRAIN_ALLOCATED(plugin)
    &&  ACTIVATED
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(filename))
    {
        GList* element = g_list_find(NETWORKS, network);
        if (BRAIN_ALLOCATED(element))
        {
            SERIALIZE(element->data, filename);
            ret = BRAIN_TRUE;
        }
    }

    return ret;
}

BrainBool
deserialize_network(const BrainPlugin plugin, const BrainNetwork network, BrainString filename)
{
    BrainBool ret = BRAIN_FALSE;

    if (BRAIN_ALLOCATED(plugin)
    &&  ACTIVATED
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(filename))
    {
        GList* element = g_list_find(NETWORKS, network);
        if (BRAIN_ALLOCATED(element))
        {
            DESERIALIZE(element->data, filename);
            ret = BRAIN_TRUE;
        }
    }

    return ret;
}

BrainString
get_plugin_author(const BrainPlugin plugin)
{
    if (BRAIN_ALLOCATED(plugin))
    {
        return AUTHOR;
    }

    return NULL;
}

BrainString
get_plugin_name(const BrainPlugin plugin)
{
    if (BRAIN_ALLOCATED(plugin))
    {
        return NAME;
    }

    return NULL;
}

BrainString
get_plugin_description(const BrainPlugin plugin)
{
    if (BRAIN_ALLOCATED(plugin))
    {
        return DESCRIPTION;
    }

    return NULL;
}

BrainString
get_plugin_date(const BrainPlugin plugin)
{
    if (BRAIN_ALLOCATED(plugin))
    {
        return DATE;
    }

    return NULL;
}

BrainBool
is_plugin_activated(const BrainPlugin plugin)
{
    BrainBool ret = BRAIN_FALSE;

    if (BRAIN_ALLOCATED(plugin))
    {
        ret = ACTIVATED;
    }

    return ret;
}

void
set_plugin_activated(const BrainPlugin plugin, const BrainBool activated)
{
    if (BRAIN_ALLOCATED(plugin))
    {
        ACTIVATED = activated;
    }
}
