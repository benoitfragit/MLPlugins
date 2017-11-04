#include "brain_builder.h"
#include "brain_builder_config.h"
#include "brain_logging_utils.h"
#include "brain_xml_utils.h"
#include "brain_core_types.h"
#include <dlfcn.h>

#define GET_PLUGIN_SYMBOL(handle, function) dlsym(handle, function)
#define LOAD        builder->_load
#define CONFIGURE   builder->_configure
#define SERIALIZE   builder->_serialize
#define DESERIALIZE builder->_deserialize
#define PREDICT     builder->_predict
#define TRAIN       builder->_train
#define DELETE      builder->_delete
#define LENGTH      builder->_number_of_networks
#define NETWORKS    builder->_networks
#define HANDLE      builder->_handle
#define NAME        builder->_name

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
    BUILDER_API_LOAD        = 0,
    BUILDER_API_CONFIGURE   = 1,
    BUILDER_API_SERIALIZE   = 2,
    BUILDER_API_DESERIALIZE = 3,
    BUILDER_API_TRAIN       = 4,
    BUILDER_API_PREDICT     = 5,
    BUILDER_API_DELETE      = 6,
    BUILDER_API_NONE        = 7,

    BUILDER_API_FIRST = BUILDER_API_LOAD,
    BUILDER_API_LAST  = BUILDER_API_DELETE
} PluginApiEnum;

struct Builder
{
    /******************************************************************/
    /**                     INTERNAL PARAMETERS                      **/
    /******************************************************************/
    BrainChar*              _name;
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
} Builder;

static PluginApiEnum
get_plugin_api_field(BrainString fieldname)
{
    BrainUint i = 0;
    PluginApiEnum field = BUILDER_API_NONE;

    for (i = BUILDER_API_FIRST; i <= BUILDER_API_LAST; ++i)
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
load_builder_function(BrainBuilder builder, Context context)
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
                    case BUILDER_API_LOAD:
                        LOAD       = GET_PLUGIN_SYMBOL(HANDLE, symbol);
                        break;
                    case BUILDER_API_CONFIGURE:
                        CONFIGURE   = GET_PLUGIN_SYMBOL(HANDLE, symbol);
                        break;
                    case BUILDER_API_SERIALIZE:
                        SERIALIZE   = GET_PLUGIN_SYMBOL(HANDLE, symbol);
                        break;
                    case BUILDER_API_DESERIALIZE:
                        DESERIALIZE = GET_PLUGIN_SYMBOL(HANDLE, symbol);
                        break;
                    case BUILDER_API_PREDICT:
                        PREDICT     = GET_PLUGIN_SYMBOL(HANDLE, symbol);
                        break;
                    case BUILDER_API_TRAIN:
                        TRAIN       = GET_PLUGIN_SYMBOL(HANDLE, symbol);
                        break;
                    case BUILDER_API_DELETE:
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

BrainBuilder
new_brain_builder(BrainString plugin_definition_file)
{
    BrainBuilder builder = NULL;

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

                        builder = (BrainBuilder)calloc(1, sizeof(Builder));
                        sprintf(buf, "lib%s.so", buffer);
                        HANDLE = dlopen(buf, RTLD_LAZY);
                        LENGTH = 0;
                        NETWORKS = NULL;

                        if (builder->_handle)
                        {
                            const BrainUint number_of_function = get_number_of_node_with_name(plugin_context, "function");
                            BrainUint i = 0;

                            NAME = (BrainChar*)calloc(s, sizeof(BrainChar));
                            strcpy(NAME, buffer);
                            free(buffer);

                            for (i = 0; i < number_of_function; ++i)
                            {
                                Context subcontext = get_node_with_name_and_index(plugin_context, "function", i);

                                load_builder_function(builder, subcontext);
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

                    if (builder
                    &&  LOAD
                    &&  CONFIGURE
                    &&  SERIALIZE
                    &&  DESERIALIZE
                    &&  TRAIN
                    &&  PREDICT
                    &&  DELETE)
                    {
                        BRAIN_INFO("A new builder has been created for plugin:%s\n", builder->_name);
                    }
                    else
                    {
                        delete_brain_builder(builder);

                        BRAIN_CRITICAL("Unable to create a builder\n");
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

    return builder;
}

void
delete_brain_builder(BrainBuilder builder)
{
    if (builder)
    {
        if (NAME)
        {
            free(NAME);
        }

        if (NETWORKS)
        {
            BrainUint i = 0;

            for (i = 0; i < LENGTH; ++i)
            {
                DELETE(NETWORKS[i]);
            }

            free(NETWORKS);
        }

        if (HANDLE)
        {
            dlclose(HANDLE);
        }

        free(builder);
    }
}

void
new_builder_network(BrainBuilder builder,
                    BrainString filename)
{
    if (builder)
    {
        ++LENGTH;

        NETWORKS = (BrainNetwork*)realloc(NETWORKS, LENGTH*sizeof(BrainNetwork));

        NETWORKS[LENGTH - 1] = LOAD(filename);
    }
}

void
delete_network_at_index(BrainBuilder builder,
                        const BrainUint index)
{
    if (builder
    &&  (index < LENGTH))
    {
        DELETE(NETWORKS[index]);
        --LENGTH;
    }
}

void
configure_network_at_index(BrainBuilder builder,
                            BrainString filename,
                            const BrainUint index)
{
    if (builder
    && (index < LENGTH)
    && filename)
    {
        CONFIGURE(NETWORKS[index], filename);
    }
}

void
get_network_prediction_at_index(BrainBuilder builder,
                                const BrainUint length,
                                const BrainSignal signal,
                                const BrainUint index)
{
    if (builder
    &&  (index < LENGTH)
    &&  signal)
    {
        PREDICT(NETWORKS[index], length, signal);
    }
}

void
train_network_at_index(BrainBuilder builder,
                       BrainString repository,
                       BrainString tokenizer,
                       const BrainUint index)
{
    if (builder
    &&  (index < LENGTH))
    {
        TRAIN(NETWORKS[index], repository, tokenizer);
    }
}

void
serialize_network_at_index(BrainBuilder builder,
                           BrainString filename,
                           BrainUint index)
{
    if (builder
    &&  (index < LENGTH))
    {
        SERIALIZE(NETWORKS[index], filename);
    }
}

void
deserialize_network_at_index(BrainBuilder builder,
                             BrainString filename,
                             BrainUint index)
{
    if (builder
    &&  (index < LENGTH))
    {
        DESERIALIZE(NETWORKS[index], filename);
    }
}
