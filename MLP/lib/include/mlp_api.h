#ifndef MLP_API_H
#define MLP_API_H

#include "mlp_types.h"

__declspec(dllimport)   void        mlp_plugin_init                 ();
__declspec(dllimport)   MLPMetaData mlp_plugin_metadata             ();

__declspec(dllimport)   MLPTrainer  mlp_trainer_new                 (BrainString, BrainString);
__declspec( dllimport ) void        mlp_trainer_delete              (MLPTrainer);
__declspec( dllimport ) void        mlp_trainer_configure           (MLPTrainer, BrainString);
__declspec( dllimport ) BrainBool   mlp_trainer_is_running          (MLPTrainer);
__declspec( dllimport ) BrainFloat  mlp_trainer_get_progress        (MLPTrainer);
__declspec( dllimport ) void        mlp_trainer_run                 (MLPTrainer);
__declspec( dllimport ) BrainFloat  mlp_trainer_error               (MLPTrainer);
__declspec( dllimport ) void        mlp_trainer_save_progression    (MLPTrainer, BrainString);
__declspec( dllimport ) void        mlp_trainer_restore_progression (MLPTrainer, BrainString, BrainReal, BrainReal);
__declspec( dllimport ) MLPNetwork  mlp_trainer_get_network         (MLPTrainer);
__declspec( dllimport ) BrainSignal mlp_trainer_get_layer_output_signal(MLPTrainer, BrainUint);
__declspec( dllimport ) BrainSignal mlp_trainer_get_input_signal    (MLPTrainer trainer);

__declspec( dllimport ) MLPNetwork  mlp_network_new                 (BrainString);
__declspec( dllimport ) void        mlp_network_delete              (MLPNetwork);
__declspec( dllimport ) void        mlp_network_serialize           (MLPNetwork, BrainString);
__declspec( dllimport ) void        mlp_network_deserialize         (MLPNetwork, BrainString);
__declspec( dllimport ) void        mlp_network_predict             (MLPNetwork, BrainUint, void*);
__declspec( dllimport ) BrainSignal mlp_network_get_output          (MLPNetwork);
__declspec( dllimport ) BrainUint   mlp_network_get_output_length   (MLPNetwork);
__declspec( dllimport ) BrainUint   mlp_network_get_number_of_layer (MLPNetwork);
__declspec( dllimport ) BrainUint   mlp_network_get_number_of_input (MLPNetwork);
__declspec( dllimport ) BrainUint   mlp_network_get_layer_number_of_neuron  (MLPNetwork, BrainUint);
__declspec( dllimport ) BrainSignal mlp_network_get_layer_output_signal     (MLPNetwork, BrainUint);
__declspec( dllimport ) BrainSignal mlp_network_get_input_signal            (MLPNetwork);


#endif /* MLP_API_H */
