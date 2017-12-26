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
BrainString  get_plugin_name       (const BrainPlugin plugin);
BrainString  get_plugin_date       (const BrainPlugin plugin);
BrainString  get_plugin_author     (const BrainPlugin plugin);
BrainString  get_plugin_description(const BrainPlugin plugin);
void         set_plugin_activated  (const BrainPlugin plugin,
                                    const BrainBool activated);
BrainBool    is_plugin_activated   (const BrainPlugin plugin);
BrainNetwork new_plugin_network    (const BrainPlugin plugin,
                                    BrainString filename);
BrainBool    delete_network        (const BrainPlugin plugin,
                                    BrainNetwork network);
BrainBool    configure_network     (const BrainPlugin plugin,
                                    const BrainNetwork network,
                                    BrainString filename);
BrainBool    get_network_prediction(const BrainPlugin plugin,
                                    const BrainNetwork network,
                                    const BrainUint length,
                                    const BrainSignal signal);
BrainBool    train_network         (const BrainPlugin plugin,
                                    const BrainNetwork network,
                                    BrainString repository,
                                    BrainString tokenizer,
                                    const BrainBool labelled,
                                    const BrainBool normalized);
BrainBool    serialize_network     (const BrainPlugin plugin,
                                    const BrainNetwork network,
                                    BrainString filename);
BrainBool    deserialize_network   (const BrainPlugin plugin,
                                    const BrainNetwork network,
                                    BrainString filename);
#endif /* BRAIN_PLUGIN_H */
