#ifndef MLP_API_H
#define MLP_API_H

#include "mlp_types.h"

#ifndef _GNUC
#define WINDOWS_EXPORT __declspec(dllimport)
#else
#define WINDOWS_EXPORT
#endif

WINDOWS_EXPORT void        mlp_plugin_init                 ();
WINDOWS_EXPORT MLPMetaData mlp_plugin_metadata             ();

WINDOWS_EXPORT MLPTrainer  mlp_trainer_new                 (BrainString, BrainString);
WINDOWS_EXPORT void        mlp_trainer_delete              (MLPTrainer);
WINDOWS_EXPORT void        mlp_trainer_configure           (MLPTrainer, BrainString);
WINDOWS_EXPORT BrainBool   mlp_trainer_is_running          (MLPTrainer);
WINDOWS_EXPORT BrainFloat  mlp_trainer_get_progress        (MLPTrainer);
WINDOWS_EXPORT void        mlp_trainer_run                 (MLPTrainer);
WINDOWS_EXPORT BrainFloat  mlp_trainer_error               (MLPTrainer);
WINDOWS_EXPORT void        mlp_trainer_save_progression    (MLPTrainer, BrainString);
WINDOWS_EXPORT void        mlp_trainer_restore_progression (MLPTrainer, BrainString, BrainReal, BrainReal);
WINDOWS_EXPORT MLPNetwork  mlp_trainer_get_network         (MLPTrainer);
WINDOWS_EXPORT BrainSignal mlp_trainer_get_layer_output_signal(MLPTrainer, BrainUint);
WINDOWS_EXPORT BrainSignal mlp_trainer_get_input_signal    (MLPTrainer);
WINDOWS_EXPORT BrainSignal mlp_trainer_get_target_signal   (MLPTrainer);

WINDOWS_EXPORT MLPNetwork  mlp_network_new                 (BrainString);
WINDOWS_EXPORT void        mlp_network_delete              (MLPNetwork);
WINDOWS_EXPORT void        mlp_network_serialize           (MLPNetwork, BrainString);
WINDOWS_EXPORT void        mlp_network_deserialize         (MLPNetwork, BrainString);
WINDOWS_EXPORT void        mlp_network_predict             (MLPNetwork, BrainUint, void*);
WINDOWS_EXPORT BrainSignal mlp_network_get_output          (MLPNetwork);
WINDOWS_EXPORT BrainUint   mlp_network_get_output_length   (MLPNetwork);
WINDOWS_EXPORT BrainUint   mlp_network_get_number_of_layer (MLPNetwork);
WINDOWS_EXPORT BrainUint   mlp_network_get_number_of_input (MLPNetwork);
WINDOWS_EXPORT BrainUint   mlp_network_get_layer_number_of_neuron  (MLPNetwork, BrainUint);
WINDOWS_EXPORT BrainSignal mlp_network_get_layer_output_signal     (MLPNetwork, BrainUint);
WINDOWS_EXPORT BrainSignal mlp_network_get_input_signal            (MLPNetwork);


#endif /* MLP_API_H */
