#include "mlp_api.h"

#include "mlp_trainer.h"
#include "mlp_network.h"
#include "mlp_layer.h"

#include "brain_data_utils.h"
#include "brain_memory_utils.h"
#include "brain_logging_utils.h"

#include "mlp_config.h"

#ifdef _GNUC
#define __MLP_VISIBLE__ __attribute__((visibility("default")))
#else
#define __MLP_VISIBLE__
#endif 

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


BrainSignal __MLP_VISIBLE__
mlp_trainer_get_layer_output_signal(MLPTrainer trainer, BrainUint layer_index)
{
    BrainSignal ret = NULL;

    const MLPNetwork network = get_trainer_network(trainer);
    if (BRAIN_ALLOCATED(network))
    {
        const MLPLayer layer = get_network_layer(network, layer_index);
        ret = get_layer_output(layer);
    }

    return ret;
}

BrainSignal __MLP_VISIBLE__
mlp_trainer_get_input_signal(MLPTrainer trainer)
{
    const MLPNetwork network = get_trainer_network(trainer);
    return get_network_input_signal(network);
}

BrainSignal __MLP_VISIBLE__
mlp_trainer_get_target_signal(MLPTrainer trainer)
{
    BrainSignal ret = NULL;

    if (BRAIN_ALLOCATED(trainer))
    {
        ret = get_trainer_target_signal(trainer);
    }

    return ret;
}

