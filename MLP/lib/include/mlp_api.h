#ifndef MLP_API_H
#define MLP_API_H

#include "mlp_types.h"

MLPTrainer  mlp_trainer_new                 (BrainString, BrainString);
void        mlp_trainer_delete              (void*);
void        mlp_trainer_configure           (void*, BrainString);
BrainBool   mlp_trainer_is_running          (void*);
BrainFloat  mlp_trainer_get_progress        (void*);
void        mlp_trainer_run                 (void*);
BrainFloat  mlp_trainer_error               (void*);
MLPNetwork  mlp_network_new                 (BrainString);
void        mlp_network_delete              (void*);
void        mlp_network_serialize           (void*, BrainString);
void        mlp_network_deserialize         (void*, BrainString);
void        mlp_network_predict             (void*, BrainUint, void*);
BrainSignal mlp_network_get_output          (void*);
BrainUint   mlp_network_get_output_length   (void*);

#endif /* MLP_API_H */
