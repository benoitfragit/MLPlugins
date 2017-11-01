#include "brain_builder.h"
#include "brain_builder_config.h"
#include "brain_logging_utils.h"
#include "brain_xml_utils.h"
#include <dlfcn.h>

#define GET_PLUGIN_SYMBOL(context, field, handle, function)            \
        {                                                              \
            BrainChar* xml_buffer = NULL;                              \
            BrainString keyword = get_plugin_api_keyword(field);       \
            function = NULL;                                           \
            if (keyword)                                               \
            {                                                          \
                xml_buffer = (BrainChar *)node_get_prop(context, keyword);\
                if (buffer)                                            \
                {                                                      \
                    function = dlsym(handle, xml_buffer);              \
                    free(xml_buffer);                                  \
                }                                                      \
            }                                                          \
        }

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

static BrainString
get_plugin_api_keyword(const PluginApiEnum field)
{
    BrainString keyword = NULL;
    BrainUint i = 0;

    for (i = BUILDER_API_FIRST; i <= BUILDER_API_LAST; ++i)
    {
        if (i == field)
        {
            keyword = _plugin_api_keywords[i];
            break;
        }
    }

    return keyword;
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
                    BrainChar* buffer = NULL;

                    buffer = (BrainChar *)node_get_prop(plugin_context, "name");
                    if (buffer != NULL)
                    {
                        const BrainUint s = strlen(buffer);
                        BrainChar buf[300];

                        builder = (BrainBuilder)calloc(1, sizeof(Builder));
                        sprintf(buf, "lib%s.so", buffer);
                        builder->_handle = dlopen(buf, RTLD_LAZY);

                        if (builder->_handle)
                        {
                            builder->_name = (BrainChar*)calloc(s, sizeof(BrainChar));
                            strcpy(builder->_name, buffer);
                            free(buffer);

                            GET_PLUGIN_SYMBOL(plugin_context, BUILDER_API_LOAD,         builder->_handle, builder->_load);
                            GET_PLUGIN_SYMBOL(plugin_context, BUILDER_API_CONFIGURE,    builder->_handle, builder->_configure);
                            GET_PLUGIN_SYMBOL(plugin_context, BUILDER_API_SERIALIZE,    builder->_handle, builder->_serialize);
                            GET_PLUGIN_SYMBOL(plugin_context, BUILDER_API_DESERIALIZE,  builder->_handle, builder->_deserialize);
                            GET_PLUGIN_SYMBOL(plugin_context, BUILDER_API_TRAIN,        builder->_handle, builder->_train);
                            GET_PLUGIN_SYMBOL(plugin_context, BUILDER_API_PREDICT,      builder->_handle, builder->_predict);
                            GET_PLUGIN_SYMBOL(plugin_context, BUILDER_API_DELETE,       builder->_handle, builder->_delete);
                        }
                        else
                        {
                            BRAIN_CRITICAL("Unable to open the %s plugin\n", buf);
                        }
                    }

                    if (builder
                    &&  builder->_load
                    &&  builder->_configure
                    &&  builder->_serialize
                    &&  builder->_deserialize
                    &&  builder->_train
                    &&  builder->_predict
                    &&  builder->_delete)
                    {
                        BRAIN_INFO("A new builder has been created for plugin:%s\n", builder->_name);
                    }
                    else
                    {
                        if (builder)
                        {
                            if (builder->_name)
                            {
                                free(builder->_name);
                            }

                            dlclose(builder->_handle);

                            free(builder);
                        }

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
delete_builder(BrainBuilder builder)
{
    if (builder)
    {
        if (builder->_name)
        {
            free(builder->_name);
        }

        if (builder->_handle)
        {
            dlclose(builder->_handle);
        }

        free(builder);
    }
}
