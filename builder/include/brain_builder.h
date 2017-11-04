/**
 * \file brain_builder.h
 * \brief Define the API to load a networkand  from an XML file
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to create and use a builder
 */
#ifndef BRAIN_BUILDER_H

#include "brain_builder_types.h"

BrainBuilder new_brain_builder              (BrainString plugin_definition_file);
void         delete_brain_builder           (BrainBuilder builder);
BrainUint    get_builder_number_of_networks (BrainBuilder builder);
void         new_builder_network            (BrainBuilder builder,
                                             BrainString filename);
void         delete_network_at_index        (BrainBuilder builder,
                                             const BrainUint index);
void         configure_network_at_index     (BrainBuilder builder,
                                             BrainString filename,
                                             const BrainUint index);
void         get_network_prediction_at_index(BrainBuilder builder,
                                             const BrainUint length,
                                             const BrainSignal signal,
                                             const BrainUint index);
void         train_network_at_index         (BrainBuilder builder,
                                             BrainString repository,
                                             BrainString tokenizer,
                                             const BrainUint index);
void         serialize_network_at_index     (BrainBuilder builder,
                                             BrainString filename,
                                             BrainUint index);
void         deserialize_network_at_index   (BrainBuilder builder,
                                             BrainString filename,
                                             BrainUint index);
#endif /* BRAIN_BUILDER_H */
