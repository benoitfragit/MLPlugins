#ifndef MLP_API_H
#define MLP_API_H

#include "mlp_types.h"

void        mlp_plugin_init                 ();
MLPMetaData mlp_plugin_metadata             ();
MLPTrainer  mlp_trainer_new                 (BrainString, BrainString);
void        mlp_trainer_delete              (MLPTrainer);
void        mlp_trainer_configure           (MLPTrainer, BrainString);
BrainBool   mlp_trainer_is_running          (MLPTrainer);
BrainFloat  mlp_trainer_get_progress        (MLPTrainer);
void        mlp_trainer_run                 (MLPTrainer);
BrainFloat  mlp_trainer_error               (MLPTrainer);
void        mlp_save_trainer_progression    (MLPTrainer, BrainString);
MLPNetwork  mlp_network_new                 (BrainString);
void        mlp_network_delete              (MLPNetwork);
void        mlp_network_serialize           (MLPNetwork, BrainString);
void        mlp_network_deserialize         (MLPNetwork, BrainString);
void        mlp_network_predict             (MLPNetwork, BrainUint, void*);
BrainSignal mlp_network_get_output          (MLPNetwork);
BrainUint   mlp_network_get_output_length   (MLPNetwork);

#endif /* MLP_API_H */
