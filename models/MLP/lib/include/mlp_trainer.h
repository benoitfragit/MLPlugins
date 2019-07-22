#ifndef MLP_TRAINER_H
#define MLP_TRAINER_H

#include "mlp_types.h"

MLPTrainer  new_trainer                     (MLPNetwork, MLPData);
void        configure_trainer_with_context  (MLPTrainer trainer, BrainString filepath);
void        set_trained_data                (MLPTrainer, MLPData);
void        set_trained_network             (MLPTrainer, MLPNetwork);
BrainBool   is_training_required            (const MLPTrainer);
BrainReal   get_training_progress           (const MLPTrainer);
BrainReal   step                            (MLPTrainer);

#endif /* MLP_TRAINER_H */
