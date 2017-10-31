#include "brain_builder.h"
#include "brain_builder_config.h"
#include "brain_logging_utils.h"
#include "brain_xml_utils.h"
#include <dlfcn.h>

struct Builder
{
    /******************************************************************/
    /**                     INTERNAL PARAMETERS                      **/
    /******************************************************************/
    BrainChar*              _name;
    void*                   _handle;
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
                            builder->_load        = NULL;
                            builder->_configure   = NULL;
                            builder->_serialize   = NULL;
                            builder->_deserialize = NULL;
                            builder->_train       = NULL;
                            builder->_predict     = NULL;
                            builder->_delete      = NULL;

                            builder->_name = (BrainChar*)calloc(s, sizeof(BrainChar));
                            strcpy(builder->_name, buffer);
                            free(buffer);

                            buffer = (BrainChar *)node_get_prop(plugin_context, "load");
                            if (buffer)
                            {
                                builder->_load = dlsym(builder->_handle, buffer);
                                free(buffer);
                            }
                            buffer = (BrainChar *)node_get_prop(plugin_context, "configure");
                            if (buffer)
                            {
                                builder->_configure = dlsym(builder->_handle, buffer);
                                free(buffer);
                            }
                            buffer = (BrainChar *)node_get_prop(plugin_context, "serialize");
                            if (buffer)
                            {
                                builder->_serialize = dlsym(builder->_handle, buffer);
                                free(buffer);
                            }
                            buffer = (BrainChar *)node_get_prop(plugin_context, "deserialize");
                            if (buffer)
                            {
                                builder->_deserialize = dlsym(builder->_handle, buffer);
                                free(buffer);
                            }
                            buffer = (BrainChar *)node_get_prop(plugin_context, "train");
                            if (buffer)
                            {
                                builder->_train = dlsym(builder->_handle, buffer);
                                free(buffer);
                            }
                            buffer = (BrainChar *)node_get_prop(plugin_context, "redict");
                            if (buffer)
                            {
                                builder->_predict = dlsym(builder->_handle, buffer);
                                free(buffer);
                            }
                            buffer = (BrainChar *)node_get_prop(plugin_context, "delete");
                            if (buffer)
                            {
                                builder->_delete = dlsym(builder->_handle, buffer);
                                free(buffer);
                            }
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
