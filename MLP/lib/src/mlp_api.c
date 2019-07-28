#include "mlp_api.h"

#include "mlp_trainer.h"
#include "mlp_network.h"

#include "brain_data_utils.h"
#include "brain_memory_utils.h"

#define __MLP_VISIBLE__ __attribute__((visibility("default")))

MLPTrainer __MLP_VISIBLE__
mlp_trainer_new(BrainString network_path, BrainString data_path)
{
    MLPTrainer trainer = NULL;

    MLPNetwork network = new_network_from_context(network_path);
    MLPData    data    = new_data_from_context(data_path);

    if (BRAIN_ALLOCATED(network)
    &&  BRAIN_ALLOCATED(data))
    {
        trainer = new_trainer(network, data);
    }

    return trainer;
}

void __MLP_VISIBLE__
mlp_trainer_delete(void* trainer)
{
    if (BRAIN_ALLOCATED(trainer))
    {
        delete_trainer((MLPTrainer)trainer);
    }
}

void __MLP_VISIBLE__
mlp_trainer_configure(void* trainer, BrainString path)
{
    if (BRAIN_ALLOCATED(trainer))
    {
        configure_trainer_with_context((MLPTrainer)trainer, path);
    }
}

BrainBool __MLP_VISIBLE__
mlp_trainer_is_running(void* trainer)
{
    BrainBool running = BRAIN_FALSE;

    if (BRAIN_ALLOCATED(trainer))
    {
        running = is_training_required((const MLPTrainer)trainer);
    }

    return running;
}

BrainFloat __MLP_VISIBLE__
mlp_trainer_get_progress(void* trainer)
{
    BrainFloat progress = 0.f;

    if (BRAIN_ALLOCATED(trainer))
    {
        progress = (BrainFloat)get_training_progress((const MLPTrainer)trainer);
    }

    return progress;
}

void __MLP_VISIBLE__
mlp_trainer_run(void* trainer)
{
    if (BRAIN_ALLOCATED(trainer))
    {
        step((MLPTrainer)trainer);
    }
}

BrainFloat __MLP_VISIBLE__
mlp_trainer_error(void* trainer)
{
    BrainFloat error = 1.f;

    if (BRAIN_ALLOCATED(trainer))
    {
        error = (BrainFloat)get_trainer_error((const MLPTrainer)trainer);
    }

    return error;
}

void __MLP_VISIBLE__
mlp_network_delete(void* network)
{
    if (BRAIN_ALLOCATED(network))
    {
        delete_network((MLPNetwork)network);
    }
}

MLPNetwork __MLP_VISIBLE__
mlp_network_new(BrainString path)
{
    MLPNetwork network = NULL;

    network = new_network_from_context(path);

    return network;
}

void __MLP_VISIBLE__
mlp_network_serialize(void* network, BrainString path)
{
    serialize_network((const MLPNetwork)network, path);
}

void __MLP_VISIBLE__
mlp_network_deserialize(void* network, BrainString path)
{
    deserialize_network((MLPNetwork)network, path);
}

void __MLP_VISIBLE__
mlp_network_predict(void* network, BrainUint number_of_inputs, void* signal)
{
    predict((MLPNetwork)network, number_of_inputs, (BrainSignal)signal);
}

BrainSignal __MLP_VISIBLE__
mlp_network_get_output(void* network)
{
    BrainSignal output = NULL;

    if (BRAIN_ALLOCATED(network))
    {
        output = get_network_output((const MLPNetwork)network);
    }

    return output;
}


BrainUint __MLP_VISIBLE__
mlp_network_get_output_length(void* network)
{
    return get_network_output_length((const MLPNetwork)network);
}
