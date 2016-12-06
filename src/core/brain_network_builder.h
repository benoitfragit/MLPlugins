#ifndef BRAIN_NETWORK_BUILDER_H
#define BRAIN_NETWORK_BUILDER_H

#include "brain_layer_builder.h"
#include "brain_synapse_builder.h"
#include "brain_data_reader.h"

BrainNetwork new_network_from_context(Context context);

void delete_network                 (BrainNetwork network);
void set_network_input              (BrainNetwork network, const BrainInt number_of_input, const BrainSignal in);
void initialize_network_from_context(BrainNetwork network, Context context);
void feedforward                    (BrainNetwork network);
void dump_network                   (const BrainNetwork network, BrainString filename);

BrainInt     get_network_number_of_layer     (const BrainNetwork network);
BrainInt     get_network_number_of_synapse   (const BrainNetwork network);
BrainSignal  get_network_output              (const BrainNetwork network);
BrainSynapse get_network_synapse             (const BrainNetwork network, const BrainInt layer_idx, const BrainInt neuron_idx);
BrainLayer   get_network_layer               (const BrainNetwork network, const BrainInt layer_index);
BrainResult  train(BrainNetwork network, const BrainData data, const BrainDouble target_error, const BrainInt max_iter);

#endif /* BRAIN_NETWORK_BUILDER_H */
