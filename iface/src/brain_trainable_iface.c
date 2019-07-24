#include "brain_trainable_iface.h"
#include "brain_memory_utils.h"

G_DEFINE_INTERFACE(BrainTrainable, brain_trainable, G_TYPE_OBJECT);

void
brain_trainable_default_init(BrainTrainableInterface* iface)
{
    static BrainBool initialized = BRAIN_FALSE;

    if (!initialized)
    {
        initialized = BRAIN_TRUE;
    }
}


void
brain_trainable_activate(BrainTrainable* trainable)
{
    BrainTrainableInterface* iface = NULL;

    if (IS_BRAIN_TRAINABLE(trainable))
    {
        iface = BRAIN_TRAINABLE_GET_IFACE(trainable);
        if (BRAIN_ALLOCATED(iface->activate))
        {
            iface->activate();
        }
    }
}

void
brain_trainable_deactivate(BrainTrainable* trainable)
{
    BrainTrainableInterface* iface = NULL;

    if (IS_BRAIN_TRAINABLE(trainable))
    {
        iface = BRAIN_TRAINABLE_GET_IFACE(trainable);
        if (BRAIN_ALLOCATED(iface->deactivate))
        {
            iface->deactivate();
        }
    }
}

BrainNetwork
brain_trainable_load_network(BrainTrainable* trainable, BrainString path)
{
    BrainTrainableInterface* iface = NULL;
    BrainNetwork network = NULL;

    if (IS_BRAIN_TRAINABLE(trainable)
    &&  BRAIN_ALLOCATED(path))
    {
        iface = BRAIN_TRAINABLE_GET_IFACE(trainable);
        if (BRAIN_ALLOCATED(iface->load_network))
        {
            network = iface->load_network(path);
        }
    }

    return network;
}

void
brain_trainable_configure(BrainTrainable* trainable, BrainNetwork network, BrainString path)
{
    BrainTrainableInterface* iface = NULL;

    if (IS_BRAIN_TRAINABLE(trainable)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(path))
    {
        iface = BRAIN_TRAINABLE_GET_IFACE(trainable);
        if (BRAIN_ALLOCATED(iface->configure))
        {
            iface->configure(network, path);
        }
    }
}

void
brain_trainable_serialize(BrainTrainable* trainable, const BrainNetwork network, BrainString path)
{
    BrainTrainableInterface* iface = NULL;

    if (IS_BRAIN_TRAINABLE(trainable)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(path))
    {
        iface = BRAIN_TRAINABLE_GET_IFACE(trainable);
        if (BRAIN_ALLOCATED(iface->serialize))
        {
            iface->serialize(network, path);
        }
    }
}

void
brain_trainable_deserialize(BrainTrainable* trainable, BrainNetwork network, BrainString path)
{
    BrainTrainableInterface* iface = NULL;

    if (IS_BRAIN_TRAINABLE(trainable)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(path))
    {
        iface = BRAIN_TRAINABLE_GET_IFACE(trainable);
        if (BRAIN_ALLOCATED(iface->deserialize))
        {
            iface->deserialize(network, path);
        }
    }
}

void
brain_trainable_train_from_file(BrainTrainable* trainable, BrainNetwork network, BrainString path)
{
    BrainTrainableInterface* iface = NULL;

    if (IS_BRAIN_TRAINABLE(trainable)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(path))
    {
        iface = BRAIN_TRAINABLE_GET_IFACE(trainable);
        if (BRAIN_ALLOCATED(iface->train_from_file))
        {
            iface->train_from_file(network, path);
        }
    }
}

BrainReal
brain_trainable_train(BrainTrainable* trainable, BrainNetwork network, const BrainData data)
{
    BrainTrainableInterface* iface = NULL;
    BrainReal error = 1.0;

    if (IS_BRAIN_TRAINABLE(trainable)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(data))
    {
        iface = BRAIN_TRAINABLE_GET_IFACE(trainable);
        if (BRAIN_ALLOCATED(iface->train))
        {
            error = iface->train(network, data);
        }
    }

    return error;
}

void
brain_trainable_predict(BrainTrainable* trainable, BrainNetwork network, const BrainUint number_of_input, const BrainSignal signal)
{
    BrainTrainableInterface* iface = NULL;

    if (IS_BRAIN_TRAINABLE(trainable)
    &&  BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(signal))
    {
        iface = BRAIN_TRAINABLE_GET_IFACE(trainable);
        if (BRAIN_ALLOCATED(iface->predict))
        {
            iface->predict(network, number_of_input, signal);
        }
    }
}

void
brain_trainable_delete_network(BrainTrainable* trainable, BrainNetwork network)
{
    BrainTrainableInterface* iface = NULL;

    if (IS_BRAIN_TRAINABLE(trainable)
    &&  BRAIN_ALLOCATED(network))
    {
        iface = BRAIN_TRAINABLE_GET_IFACE(trainable);
        if (BRAIN_ALLOCATED(iface->delete_network))
        {
            iface->delete_network(network);
        }
    }
}

BrainBool
brain_trainable_is_training_required(BrainTrainable* trainable, const BrainNetwork network)
{
    BrainTrainableInterface* iface = NULL;
    BrainBool required = BRAIN_FALSE;

    if (IS_BRAIN_TRAINABLE(trainable)
    &&  BRAIN_ALLOCATED(network))
    {
        iface = BRAIN_TRAINABLE_GET_IFACE(trainable);
        if (BRAIN_ALLOCATED(iface->is_training_required))
        {
            required = iface->is_training_required(network);
        }
    }

    return required;
}

BrainData
brain_trainable_load_data(BrainTrainable* trainable, BrainString path)
{
    BrainTrainableInterface* iface = NULL;
    BrainData data = NULL;

    if (IS_BRAIN_TRAINABLE(trainable)
    &&  BRAIN_ALLOCATED(path))
    {
        iface = BRAIN_TRAINABLE_GET_IFACE(trainable);
        if (BRAIN_ALLOCATED(iface->load_data))
        {
            data = iface->load_data(path);
        }
    }

    return data;
}

void
brain_trainable_delete_data(BrainTrainable* trainable, BrainData data)
{
    BrainTrainableInterface* iface = NULL;

    if (IS_BRAIN_TRAINABLE(trainable)
    &&  BRAIN_ALLOCATED(data))
    {
        iface = BRAIN_TRAINABLE_GET_IFACE(trainable);
        if (BRAIN_ALLOCATED(iface->delete_data))
        {
            iface->delete_data(data);
        }
    }
}

BrainReal
brain_trainable_get_progress(BrainTrainable* trainable, BrainNetwork network)
{
    BrainTrainableInterface* iface = NULL;
    BrainReal progress = 0.;

    if (IS_BRAIN_TRAINABLE(trainable)
    &&  BRAIN_ALLOCATED(network))
    {
        iface = BRAIN_TRAINABLE_GET_IFACE(trainable);
        if (BRAIN_ALLOCATED(iface->get_progress))
        {
            progress = iface->get_progress(network);
        }
    }

    return progress;
}
