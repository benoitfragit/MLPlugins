#ifndef MLP_TRAINER_H
#define MLP_TRAINER_H

#include "mlp_types.h"

MLPTrainer  new_trainer                     (MLPNetwork, MLPData);
void        delete_trainer                  (MLPTrainer);
void        configure_trainer_with_context  (MLPTrainer, BrainString);
BrainBool   is_training_required            (const MLPTrainer);
BrainReal   get_training_progress           (const MLPTrainer);
void        step                            (MLPTrainer);
BrainReal   get_trainer_error               (const MLPTrainer);
#endif /* MLP_TRAINER_H */
