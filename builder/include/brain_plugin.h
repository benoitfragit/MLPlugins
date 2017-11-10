/**
 * \file brain_plugin.h
 * \brief Define the API to load a networkand  from an XML file
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to create and use a plugin
 */
#ifndef BRAIN_PLUGIN_H

#include "brain_plugin_types.h"

BrainPlugin new_brain_plugin              (BrainString plugin_definition_file);
void         delete_brain_plugin           (BrainPlugin plugin);
BrainUint    get_plugin_number_of_networks (BrainPlugin plugin);
void         new_plugin_network            (BrainPlugin plugin,
                                             BrainString filename);
void         delete_network_at_index        (BrainPlugin plugin,
                                             const BrainUint index);
void         configure_network_at_index     (BrainPlugin plugin,
                                             BrainString filename,
                                             const BrainUint index);
void         get_network_prediction_at_index(BrainPlugin plugin,
                                             const BrainUint length,
                                             const BrainSignal signal,
                                             const BrainUint index);
void         train_network_at_index         (BrainPlugin plugin,
                                             BrainString repository,
                                             BrainString tokenizer,
                                             const BrainUint index);
void         serialize_network_at_index     (BrainPlugin plugin,
                                             BrainString filename,
                                             BrainUint index);
void         deserialize_network_at_index   (BrainPlugin plugin,
                                             BrainString filename,
                                             BrainUint index);
#endif /* BRAIN_PLUGIN_H */
