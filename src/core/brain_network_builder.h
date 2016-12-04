#ifndef BRAIN_NETWORK_BUILDER_H
#define BRAIN_NETWORK_BUILDER_H

#include "brain_layer_builder.h"
#include "brain_synapse_builder.h"

BrainNetwork new_network_from_context(Context context);

void delete_network                 (BrainNetwork network);
void set_network_output             (BrainNetwork network, const BrainInt output_index, const BrainDouble value);
void set_network_input              (BrainNetwork network, const BrainInt number_of_input, const BrainDouble *in);
void update_network_weight          (BrainNetwork network);
void activate_network_synapse       (BrainNetwork network);
void update_network_output          (BrainNetwork network);
void initialize_network_from_context(BrainNetwork network, Context context);

BrainSynapse       new_network_synapse_from_context(const BrainNetwork network, Context context);
void               dump_network                    (const BrainNetwork network, BrainString filename);
BrainInt           get_network_number_of_layer     (const BrainNetwork network);
BrainInt           get_network_number_of_synapse   (const BrainNetwork network);
const BrainDouble* get_network_output              (const BrainNetwork network);
BrainSynapse       get_network_synapse_with_index  (const BrainNetwork network, const BrainInt index);
BrainSynapse       get_network_synapse             (const BrainNetwork network, const BrainInt layer_idx, const BrainInt neuron_idx);
BrainLayer         get_network_layer               (const BrainNetwork network, const BrainInt layer_index);

#endif /* BRAIN_NETWORK_BUILDER_H */
