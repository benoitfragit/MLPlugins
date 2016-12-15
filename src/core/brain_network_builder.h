#ifndef BRAIN_NETWORK_BUILDER_H
#define BRAIN_NETWORK_BUILDER_H

#include "brain_layer_builder.h"
#include "brain_data_reader.h"

BrainNetwork new_network_from_context(Context context);

void delete_network(BrainNetwork network);
void initialize_network_from_context(BrainNetwork network,
                                     Context context);
void feedforward(BrainNetwork network,
                 const BrainUint number_of_input,
                 const BrainSignal in);
void dump_network(const BrainNetwork network,
                  BrainString filename);
BrainUint   get_network_number_of_layer(const BrainNetwork network);
BrainSignal get_network_output(const BrainNetwork network);
BrainLayer  get_network_layer(const BrainNetwork network,
                              const BrainUint layer_index);
BrainResult train(BrainNetwork      network,
                  const BrainData   data,
                  const BrainDouble target_error,
                  const BrainUint   max_iter);

#endif /* BRAIN_NETWORK_BUILDER_H */
