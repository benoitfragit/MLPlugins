#ifndef BRAIN_NETWORK_BUILDER_H
#define BRAIN_NETWORK_BUILDER_H

#include "brain_layer_builder.h"
#include "brain_synapse_builder.h"

typedef struct Network* Network_t;

Network_t     new_network_from_context(Context context);

void          delete_network                  (Network_t network);
void          set_network_output              (Network_t network, const int output_index, const double value);
void          set_network_input               (Network_t network, const int number_of_input, const double *in);
void          update_network_weight           (Network_t network);
void          activate_network_synapse        (Network_t network);
void          update_network_output           (Network_t network);
void          initialize_network_from_context (Network_t network, Context context);

Synapse_t     new_network_synapse_from_context(const Network_t network, Context context);
void          dump_network                    (const Network_t network, const char* filename);
int           get_network_number_of_layer     (const Network_t network);
int           get_network_number_of_synapse   (const Network_t network);
const double* get_network_output              (const Network_t network);
Synapse_t     get_network_synapse_with_index  (const Network_t network, const int index);
Synapse_t     get_network_synapse             (const Network_t network, const int layer_idx, const int neuron_idx);
Layer_t       get_network_layer               (const Network_t network, const int layer_index);

#endif /* BRAIN_NETWORK_BUILDER_H */
