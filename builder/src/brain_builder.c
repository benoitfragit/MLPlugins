#include "brain_builder.h"
#include "brain_logging_utils.h"
#include <dlfcn.h>

struct Builder
{
    BrainLoaderPtrFunc      load;
    BrainConfigurePtrFunc   configure;
    BrainSerializePtrFunc   serialize;
    BrainDeserializePtrFunc deserialize;
    BrainTrainPtrFunc       train;
    BrainPredictPtrFunc     predict;
} Builder;

BrainBuilder
new_brain_builder(BrainString plugin,
                  BrainString plugin_load_function,
                  BrainString plugin_configure_function,
                  BrainString plugin_serialize_function,
                  BrainString plugin_deserialize_function,
                  BrainString plugin_train_function,
                  BrainString plugin_predict_function)
{
    BrainBuilder builder = NULL;

    if (plugin
    &&  plugin_load_function
    &&  plugin_configure_function
    &&  plugin_serialize_function
    &&  plugin_deserialize_function
    &&  plugin_train_function)
    {
        void* handle = NULL;
        char buf[300];

        sprintf(buf, "lib%s.so", plugin);

        handle = dlopen(buf, RTLD_LAZY);

        if (handle)
        {
            BrainLoaderPtrFunc      load        = dlsym(handle, plugin_load_function);
            BrainConfigurePtrFunc   configure   = dlsym(handle, plugin_configure_function);
            BrainSerializePtrFunc   serialize   = dlsym(handle, plugin_serialize_function);
            BrainDeserializePtrFunc deserialize = dlsym(handle, plugin_deserialize_function);
            BrainTrainPtrFunc       train       = dlsym(handle, plugin_train_function);
            BrainPredictPtrFunc     predict     = dlsym(handle, plugin_predict_function);

            if (load
            &&  configure
            &&  serialize
            &&  deserialize
            &&  train
            &&  predict)
            {
                builder                 = (BrainBuilder)calloc(1, sizeof(Builder));

                builder->load           = load;
                builder->configure      = configure;
                builder->serialize      = serialize;
                builder->deserialize    = deserialize;
                builder->train          = train;
                builder->predict        = predict;
            }

            dlclose(handle);
        }
    }

    if (!builder)
    {
        BRAIN_WARNING("Unable to load plugin:%s\n", plugin);
    }
    else
    {
        BRAIN_INFO("Plugin:%s has been loaded\n", plugin);
    }

    return builder;
}
