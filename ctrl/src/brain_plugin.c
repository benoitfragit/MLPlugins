#include "brain_plugin.h"
#include "brain_plugin_config.h"
#include "brain_logging_utils.h"
#include "brain_xml_utils.h"
#include "brain_core_types.h"
#include <dlfcn.h>

#define GET_PLUGIN_SYMBOL(handle, function) dlsym(handle, function)

#define LOAD        plugin->_load
#define CONFIGURE   plugin->_configure
#define SERIALIZE   plugin->_serialize
#define DESERIALIZE plugin->_deserialize
#define PREDICT     plugin->_predict
#define TRAIN       plugin->_train
#define DELETE      plugin->_delete
#define LENGTH      plugin->_number_of_networks
#define NETWORKS    plugin->_networks
#define HANDLE      plugin->_handle
#define NAME        plugin->_name
#define AUTHOR      plugin->_author
#define DESCRIPTION plugin->_description
#define DATE        plugin->_date

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

struct Plugin
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
    BrainHandle             _handle;
    BrainNetwork*           _networks;
    BrainUint               _number_of_networks;
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
    if (context)
    {
        BrainChar* buffer = (BrainChar*)node_get_prop(context, "name");

        if (buffer)
        {
            const PluginApiEnum field = get_plugin_api_field(buffer);
            BrainChar* symbol = (BrainChar*)node_get_prop(context, "symbol");

            if (symbol)
            {
                switch(field)
                {
                    case PLUGIN_API_LOAD:
                        LOAD        = GET_PLUGIN_SYMBOL(HANDLE, symbol);
                        break;
                    case PLUGIN_API_CONFIGURE:
                        CONFIGURE   = GET_PLUGIN_SYMBOL(HANDLE, symbol);
                        break;
                    case PLUGIN_API_SERIALIZE:
                        SERIALIZE   = GET_PLUGIN_SYMBOL(HANDLE, symbol);
                        break;
                    case PLUGIN_API_DESERIALIZE:
                        DESERIALIZE = GET_PLUGIN_SYMBOL(HANDLE, symbol);
                        break;
                    case PLUGIN_API_PREDICT:
                        PREDICT     = GET_PLUGIN_SYMBOL(HANDLE, symbol);
                        break;
                    case PLUGIN_API_TRAIN:
                        TRAIN       = GET_PLUGIN_SYMBOL(HANDLE, symbol);
                        break;
                    case PLUGIN_API_DELETE:
                        DELETE      = GET_PLUGIN_SYMBOL(HANDLE, symbol);
                        break;
                    default:
                        break;
                }

                free(symbol);
            }

            free(buffer);
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

    if (plugin_definition_file)
    {
        if (validate_with_xsd(plugin_definition_file, PLUGIN_DEFINITION_XSD))
        {
            Document plugin_document = open_document(plugin_definition_file);

            if (plugin_document)
            {
                Context plugin_context = get_root_node(plugin_document);

                if (plugin_context && is_node_with_name(plugin_context, "plugin"))
                {
                    BrainChar* buffer = (BrainChar *)node_get_prop(plugin_context, "name");

                    if (buffer != NULL)
                    {
                        const BrainUint s = strlen(buffer);
                        BrainChar buf[300];

                        plugin = (BrainPlugin)calloc(1, sizeof(Plugin));

                        sprintf(buf, "lib%s.so", buffer);
                        HANDLE = dlopen(buf, RTLD_LAZY);
                        LENGTH = 0;
                        NETWORKS = NULL;
                        NAME     = NULL;
                        DESCRIPTION = NULL;
                        AUTHOR = NULL;

                        if (HANDLE)
                        {
                            const BrainUint number_of_function = get_number_of_node_with_name(plugin_context, "function");
                            BrainUint i = 0;

                            NAME = (BrainChar*)calloc(s, sizeof(BrainChar));
                            strcpy(NAME, buffer);
                            free(buffer);

                            buffer = (BrainChar *)node_get_prop(plugin_context, "author");
                            if (buffer)
                            {
                                AUTHOR = (BrainChar *)calloc(strlen(buffer), sizeof(BrainChar));
                                strcpy(AUTHOR, buffer);
                                free(buffer);
                            }
                            buffer = (BrainChar *)node_get_prop(plugin_context, "description");
                            if (buffer)
                            {
                                DESCRIPTION = (BrainChar *)calloc(strlen(buffer), sizeof(BrainChar));
                                strcpy(DESCRIPTION, buffer);
                                free(buffer);
                            }
                            buffer = (BrainChar *)node_get_prop(plugin_context, "date");
                            if (buffer)
                            {
                                DATE = (BrainChar *)calloc(strlen(buffer), sizeof(BrainChar));
                                strcpy(DATE, buffer);
                                free(buffer);
                            }

                            for (i = 0; i < number_of_function; ++i)
                            {
                                Context subcontext = get_node_with_name_and_index(plugin_context, "function", i);

                                load_plugin_function(plugin, subcontext);
                            }
                        }
                        else
                        {
                            BRAIN_CRITICAL("Unable to open the %s plugin\n", buf);
                        }
                    }
                    else
                    {
                        BRAIN_CRITICAL("Plugin name not loaded\n");
                    }

                    if (isPluginValid(plugin))
                    {
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
    if (plugin)
    {
        // Removing all attached networks
        if (NETWORKS)
        {
            BrainUint i = 0;

            for (i = 0; i < LENGTH; ++i)
            {
                DELETE(NETWORKS[i]);
            }

            free(NETWORKS);
        }

        // Closing the handle
        if (HANDLE)
        {
            dlclose(HANDLE);
        }

        // Cleaning all metadata
        if (NAME)
        {
            free(NAME);
        }

        if (DATE)
        {
            free(DATE);
        }

        if (DESCRIPTION)
        {
            free(DESCRIPTION);
        }

        if (AUTHOR)
        {
            free(AUTHOR);
        }

        // finally free the plugin itself
        free(plugin);
    }
}

BrainNetwork
new_plugin_network(const BrainPlugin plugin, BrainString filename)
{
    BrainNetwork network = NULL;

    if (plugin)
    {
        network = LOAD(filename);

        if (network != NULL)
        {
            ++LENGTH;

            NETWORKS = (BrainNetwork*)realloc(NETWORKS, LENGTH*sizeof(BrainNetwork));

            NETWORKS[LENGTH - 1] = network;
        }
    }

    return network;
}

void
delete_network_at_index(const BrainPlugin plugin, const BrainUint index)
{
    if (plugin
    &&  (index < LENGTH))
    {
        DELETE(NETWORKS[index]);
        --LENGTH;
    }
}

void
configure_network_at_index(const BrainPlugin plugin, BrainString filename, const BrainUint index)
{
    if (plugin
    && (index < LENGTH)
    && filename)
    {
        CONFIGURE(NETWORKS[index], filename);
    }
}

void
get_network_prediction_at_index(const BrainPlugin plugin, const BrainUint length, const BrainSignal signal, const BrainUint index)
{
    if (plugin
    &&  (index < LENGTH)
    &&  signal)
    {
        PREDICT(NETWORKS[index], length, signal);
    }
}

void
train_network_at_index(const BrainPlugin plugin, BrainString repository, BrainString tokenizer, const BrainBool labelled, const BrainBool normalized, const BrainUint index)
{
    if (plugin
    &&  (index < LENGTH))
    {
        TRAIN(NETWORKS[index], repository, tokenizer, labelled, normalized);
    }
}

void
serialize_network_at_index(const BrainPlugin plugin, BrainString filename, const BrainUint index)
{
    if (plugin
    &&  (index < LENGTH))
    {
        SERIALIZE(NETWORKS[index], filename);
    }
}

void
deserialize_network_at_index(const BrainPlugin plugin, BrainString filename, const BrainUint index)
{
    if (plugin
    &&  (index < LENGTH))
    {
        DESERIALIZE(NETWORKS[index], filename);
    }
}

BrainUint
get_plugin_number_of_networks(const BrainPlugin plugin)
{
    BrainUint ret = 0;

    if (plugin)
    {
        ret = LENGTH;
    }

    return ret;
}

BrainString
get_plugin_author(const BrainPlugin plugin)
{
    if (plugin)
    {
        return AUTHOR;
    }

    return NULL;
}

BrainString
get_plugin_name(const BrainPlugin plugin)
{
    if (plugin)
    {
        return NAME;
    }

    return NULL;
}

BrainString
get_plugin_description(const BrainPlugin plugin)
{
    if (plugin)
    {
        return DESCRIPTION;
    }

    return NULL;
}

BrainString
get_plugin_date(const BrainPlugin plugin)
{
    if (plugin)
    {
        return DATE;
    }

    return NULL;
}
