/**
 * \file brain_plugin.h
 * \brief Define the API to load a networkand  from an XML file
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to create and use a plugin
 */
#ifndef BRAIN_PLUGIN_H
#include "brain_trainer_types.h"

BrainPlugin  new_plugin            (BrainString plugin_definition_file);
void         delete_plugin         (BrainPlugin plugin);
BrainNetwork new_plugin_network    (const BrainPlugin plugin,
                                    BrainString filename);
BrainData    new_plugin_data       (const BrainPlugin,
                                    BrainString filename);
void         delete_plugin_network (const BrainPlugin plugin,
                                    BrainNetwork network);
void         delete_plugin_data    (const BrainPlugin plugin,
                                    BrainData data);
void         configure_network     (const BrainPlugin plugin,
                                    BrainNetwork network,
                                    BrainString filename);
void         get_network_prediction(const BrainPlugin plugin,
                                    BrainNetwork network,
                                    const BrainUint length,
                                    const BrainSignal signal);
void         train_network_from_file(const BrainPlugin plugin,
                                    BrainNetwork network,
                                    BrainString datapath);
BrainReal    train_network_step     (const BrainPlugin plugin,
                                     BrainNetwork network,
                                     const BrainData data);
BrainBool    is_training_required   (const BrainPlugin plugin,
                                     const BrainNetwork network);
void         serialize_network     (const BrainPlugin plugin,
                                    BrainNetwork network,
                                    BrainString filename);
void         deserialize_network   (const BrainPlugin plugin,
                                    BrainNetwork network,
                                    BrainString filename);
BrainReal    get_training_progress(const BrainPlugin plugin,
                                   const BrainNetwork network);
#endif /* BRAIN_PLUGIN_H */
