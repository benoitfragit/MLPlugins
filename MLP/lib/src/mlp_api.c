#include "mlp_api.h"

#include "mlp_trainer.h"
#include "mlp_network.h"
#include "mlp_layer.h"

#include "brain_data_utils.h"
#include "brain_memory_utils.h"
#include "brain_logging_utils.h"

#include "mlp_config.h"

#define __MLP_VISIBLE__ __attribute__((visibility("default")))

void __MLP_VISIBLE__
mlp_plugin_init()
{
    brain_logging_init();
}

MLPMetaData __MLP_VISIBLE__
mlp_plugin_metadata()
{
    return &mlpInfos;
}

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
mlp_trainer_delete(MLPTrainer trainer)
{
    if (BRAIN_ALLOCATED(trainer))
    {
        delete_trainer(trainer);
    }
}

void __MLP_VISIBLE__
mlp_trainer_configure(MLPTrainer trainer, BrainString path)
{
    if (BRAIN_ALLOCATED(trainer))
    {
        configure_trainer_with_context(trainer, path);
    }
}

BrainBool __MLP_VISIBLE__
mlp_trainer_is_running(MLPTrainer trainer)
{
    BrainBool running = BRAIN_FALSE;

    if (BRAIN_ALLOCATED(trainer))
    {
        running = is_training_required(trainer);
    }

    return running;
}

BrainFloat __MLP_VISIBLE__
mlp_trainer_get_progress(MLPTrainer trainer)
{
    BrainFloat progress = 0.f;

    if (BRAIN_ALLOCATED(trainer))
    {
        progress = (BrainFloat)get_training_progress(trainer);
    }

    return progress;
}

void __MLP_VISIBLE__
mlp_trainer_run(MLPTrainer trainer)
{
    if (BRAIN_ALLOCATED(trainer))
    {
        step(trainer);
    }
}

BrainFloat __MLP_VISIBLE__
mlp_trainer_error(MLPTrainer trainer)
{
    BrainFloat error = 1.f;

    if (BRAIN_ALLOCATED(trainer))
    {
        error = (BrainFloat)get_trainer_error(trainer);
    }

    return error;
}

void __MLP_VISIBLE__
mlp_trainer_save_progression(MLPTrainer trainer, BrainString path)
{
    if (BRAIN_ALLOCATED(trainer) && BRAIN_ALLOCATED(path))
    {
        save_trainer_progression(trainer, path);
    }
}

void __MLP_VISIBLE__
mlp_trainer_restore_progression(MLPTrainer trainer,
                                BrainString serialized_network,
                                BrainReal progress,
                                BrainReal error)
{
    if (BRAIN_ALLOCATED(trainer) &&
        BRAIN_ALLOCATED(serialized_network) )
    {
        restore_trainer_progression(trainer, serialized_network, progress, error);
    }
}

MLPNetwork __MLP_VISIBLE__
mlp_trainer_get_network(MLPTrainer trainer)
{
    MLPNetwork ret = NULL;

    if (BRAIN_ALLOCATED(trainer))
    {
        ret = get_trainer_network(trainer);
    }

    return ret;
}

void __MLP_VISIBLE__
mlp_network_delete(MLPNetwork network)
{
    if (BRAIN_ALLOCATED(network))
    {
        delete_network(network);
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
mlp_network_serialize(MLPNetwork network, BrainString path)
{
    serialize_network(network, path);
}

void __MLP_VISIBLE__
mlp_network_deserialize(MLPNetwork network, BrainString path)
{
    deserialize_network(network, path);
}

void __MLP_VISIBLE__
mlp_network_predict(MLPNetwork network, BrainUint number_of_inputs, void* signal)
{
    predict(network, number_of_inputs, (BrainSignal)signal);
}

BrainSignal __MLP_VISIBLE__
mlp_network_get_output(MLPNetwork network)
{
    BrainSignal output = NULL;

    if (BRAIN_ALLOCATED(network))
    {
        output = get_network_output(network);
    }

    return output;
}

BrainUint __MLP_VISIBLE__
mlp_network_get_output_length(MLPNetwork network)
{
    return get_network_output_length(network);
}

BrainUint __MLP_VISIBLE__
mlp_network_get_number_of_layer(MLPNetwork network)
{
    return get_network_number_of_layer(network);
}

BrainUint __MLP_VISIBLE__
mlp_network_get_number_of_input(MLPNetwork network)
{
    return get_network_number_of_input(network);
}

BrainUint __MLP_VISIBLE__
mlp_network_get_layer_number_of_neuron(MLPNetwork network, BrainUint layer_index)
{
    BrainUint ret = 0;

    const MLPLayer layer = get_network_layer(network, layer_index);
    if (BRAIN_ALLOCATED(layer))
    {
        ret = get_layer_number_of_neuron(layer);
    }

    return ret;
}
