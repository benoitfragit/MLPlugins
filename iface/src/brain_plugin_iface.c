#include "brain_plugin_iface.h"
#include "brain_memory_utils.h"

G_DEFINE_INTERFACE(BrainPlugin, brain_plugin, G_TYPE_OBJECT);

void
brain_plugin_default_init(BrainPluginInterface* iface)
{
    static BrainBool initialized = BRAIN_FALSE;

    if (!initialized)
    {
        initialized = BRAIN_TRUE;
    }
}


void
brain_plugin_activate(BrainPlugin* plugin)
{
    BrainPluginInterface* iface = NULL;

    if (IS_BRAIN_PLUGIN(plugin))
    {
        iface = BRAIN_PLUGIN_GET_IFACE(plugin);
        if (BRAIN_ALLOCATED(iface->activate))
        {
            iface->activate();
        }
    }
}

void
brain_plugin_deactivate(BrainPlugin* plugin)
{
    BrainPluginInterface* iface = NULL;

    if (IS_BRAIN_PLUGIN(plugin))
    {
        iface = BRAIN_PLUGIN_GET_IFACE(plugin);
        if (BRAIN_ALLOCATED(iface->deactivate))
        {
            iface->deactivate();
        }
    }
}

BrainNetwork
brain_plugin_load_network(BrainPlugin* plugin, BrainString path)
{
    BrainPluginInterface* iface = NULL;
    BrainNetwork network = NULL;

    if (IS_BRAIN_PLUGIN(plugin)
    &&  BRAIN_ALLOCATED(path))
    {
        iface = BRAIN_PLUGIN_GET_IFACE(plugin);
        if (BRAIN_ALLOCATED(iface->load_network))
        {
            network = iface->load_network(path);
        }
    }

    return network;
}

void
brain_plugin_configure(BrainPlugin* plugin, BrainNetwork network, BrainString path)
{
    BrainPluginInterface* iface = NULL;

    if (IS_BRAIN_PLUGIN(plugin)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(path))
    {
        iface = BRAIN_PLUGIN_GET_IFACE(plugin);
        if (BRAIN_ALLOCATED(iface->configure))
        {
            iface->configure(network, path);
        }
    }
}

void
brain_plugin_serialize(BrainPlugin* plugin, const BrainNetwork network, BrainString path)
{
    BrainPluginInterface* iface = NULL;

    if (IS_BRAIN_PLUGIN(plugin)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(path))
    {
        iface = BRAIN_PLUGIN_GET_IFACE(plugin);
        if (BRAIN_ALLOCATED(iface->serialize))
        {
            iface->serialize(network, path);
        }
    }
}

void
brain_plugin_deserialize(BrainPlugin* plugin, BrainNetwork network, BrainString path)
{
    BrainPluginInterface* iface = NULL;

    if (IS_BRAIN_PLUGIN(plugin)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(path))
    {
        iface = BRAIN_PLUGIN_GET_IFACE(plugin);
        if (BRAIN_ALLOCATED(iface->deserialize))
        {
            iface->deserialize(network, path);
        }
    }
}

void
brain_plugin_train_from_file(BrainPlugin* plugin, BrainNetwork network, BrainString path)
{
    BrainPluginInterface* iface = NULL;

    if (IS_BRAIN_PLUGIN(plugin)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(path))
    {
        iface = BRAIN_PLUGIN_GET_IFACE(plugin);
        if (BRAIN_ALLOCATED(iface->train_from_file))
        {
            iface->train_from_file(network, path);
        }
    }
}

BrainReal
brain_plugin_train(BrainPlugin* plugin, BrainNetwork network, const BrainData data)
{
    BrainPluginInterface* iface = NULL;

    if (IS_BRAIN_PLUGIN(plugin)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(data))
    {
        iface = BRAIN_PLUGIN_GET_IFACE(plugin);
        if (BRAIN_ALLOCATED(iface->train))
        {
            iface->train(network, data);
        }
    }
}

void
brain_plugin_predict(BrainPlugin* plugin, BrainNetwork network, const BrainUint number_of_input, const BrainSignal signal)
{
    BrainPluginInterface* iface = NULL;

    if (IS_BRAIN_PLUGIN(plugin)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(signal))
    {
        iface = BRAIN_PLUGIN_GET_IFACE(plugin);
        if (BRAIN_ALLOCATED(iface->predict))
        {
            iface->predict(network, number_of_input, signal);
        }
    }
}

void
brain_plugin_delete_network(BrainPlugin* plugin, BrainNetwork network)
{
    BrainPluginInterface* iface = NULL;

    if (IS_BRAIN_PLUGIN(plugin)
    &&  BRAIN_ALLOCATED(network))
    {
        iface = BRAIN_PLUGIN_GET_IFACE(plugin);
        if (BRAIN_ALLOCATED(iface->delete_network))
        {
            iface->delete_network(network);
        }
    }
}

BrainBool
brain_plugin_is_training_required(BrainPlugin* plugin, const BrainNetwork network)
{
    BrainPluginInterface* iface = NULL;
    BrainBool required = BRAIN_FALSE;

    if (IS_BRAIN_PLUGIN(plugin)
    &&  BRAIN_ALLOCATED(network))
    {
        iface = BRAIN_PLUGIN_GET_IFACE(plugin);
        if (BRAIN_ALLOCATED(iface->is_training_required))
        {
            required = iface->is_training_required(network);
        }
    }

    return required;
}

BrainData
brain_plugin_load_data(BrainPlugin* plugin, BrainString path)
{
    BrainPluginInterface* iface = NULL;
    BrainData data = NULL;

    if (IS_BRAIN_PLUGIN(plugin)
    &&  BRAIN_ALLOCATED(path))
    {
        iface = BRAIN_PLUGIN_GET_IFACE(plugin);
        if (BRAIN_ALLOCATED(iface->load_data))
        {
            data = iface->load_data(path);
        }
    }

    return data;
}

void
brain_plugin_delete_data(BrainPlugin* plugin, BrainData data)
{
    BrainPluginInterface* iface = NULL;

    if (IS_BRAIN_PLUGIN(plugin)
    &&  BRAIN_ALLOCATED(data))
    {
        iface = BRAIN_PLUGIN_GET_IFACE(plugin);
        if (BRAIN_ALLOCATED(iface->delete_data))
        {
            iface->delete_data(data);
        }
    }
}

BrainReal
brain_plugin_get_progress(BrainPlugin* plugin, BrainNetwork network)
{
    BrainPluginInterface* iface = NULL;
    BrainReal progress = 0.;

    if (IS_BRAIN_PLUGIN(plugin)
    &&  BRAIN_ALLOCATED(network))
    {
        iface = BRAIN_PLUGIN_GET_IFACE(plugin);
        if (BRAIN_ALLOCATED(iface->get_progress))
        {
            progress = iface->get_progress(network);
        }
    }

    return progress;
}
