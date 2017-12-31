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

BrainPlugin  new_plugin            (BrainString plugin_definition_file);
void         delete_plugin         (BrainPlugin plugin);
BrainNetwork new_plugin_network    (const BrainPlugin plugin,
                                    BrainString filename);
void         delete_network        (const BrainPlugin plugin,
                                    BrainNetwork network);
void         configure_network     (const BrainPlugin plugin,
                                    BrainNetwork network,
                                    BrainString filename);
void         get_network_prediction(const BrainPlugin plugin,
                                    BrainNetwork network,
                                    const BrainUint length,
                                    const BrainSignal signal);
void         train_network         (const BrainPlugin plugin,
                                    BrainNetwork network,
                                    BrainString repository,
                                    BrainString tokenizer,
                                    const BrainBool labelled,
                                    const BrainBool normalized);
void         serialize_network     (const BrainPlugin plugin,
                                    BrainNetwork network,
                                    BrainString filename);
void         deserialize_network   (const BrainPlugin plugin,
                                    BrainNetwork network,
                                    BrainString filename);
#endif /* BRAIN_PLUGIN_H */
