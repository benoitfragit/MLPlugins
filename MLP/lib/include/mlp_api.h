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
void        mlp_trainer_save_progression    (MLPTrainer, BrainString);
void        mlp_trainer_restore_progression (MLPTrainer, BrainString, BrainReal, BrainReal);
MLPNetwork  mlp_trainer_get_network         (MLPTrainer);
BrainSignal mlp_trainer_get_layer_output_signal(MLPTrainer, BrainUint);
BrainSignal mlp_trainer_get_input_signal    (MLPTrainer trainer);

MLPNetwork  mlp_network_new                 (BrainString);
void        mlp_network_delete              (MLPNetwork);
void        mlp_network_serialize           (MLPNetwork, BrainString);
void        mlp_network_deserialize         (MLPNetwork, BrainString);
void        mlp_network_predict             (MLPNetwork, BrainUint, void*);
BrainSignal mlp_network_get_output          (MLPNetwork);
BrainUint   mlp_network_get_output_length   (MLPNetwork);
BrainUint   mlp_network_get_number_of_layer (MLPNetwork);
BrainUint   mlp_network_get_number_of_input (MLPNetwork);
BrainUint   mlp_network_get_layer_number_of_neuron  (MLPNetwork, BrainUint);
BrainSignal mlp_network_get_layer_output_signal     (MLPNetwork, BrainUint);
BrainSignal mlp_network_get_input_signal            (MLPNetwork);


#endif /* MLP_API_H */
