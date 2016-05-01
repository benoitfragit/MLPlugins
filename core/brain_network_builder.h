#ifndef BRAIN_NETWORK_BUILDER_H
#define BRAIN_NETWORK_BUILDER_H

#include "brain_layer_builder.h"
#include "brain_synapse_builder.h"

typedef struct Network* Network_t;

Network_t  new_network_from_context(Context context);
void      delete_network(Network_t network);
Layer_t   layer(Network_t network, const int layer_index);
Synapse_t synapse(Network_t network, const int layer_id, const int neuron_id);
Synapse_t synapse_with_index(Network_t network, const int index);
void set_output(Network_t network, const int output_index, const double value);
const double* get_output(const Network_t network);
void set_network_input(Network_t network, const int number_of_input, const double *in);
void dump_network(const Network_t network, const char* filename);
int get_number_of_layer(const Network_t network);
int get_number_of_synapse(const Network_t network);
void set_trained(Network_t network, const int trained);
int  is_trained(const Network_t network);
#endif /* BRAIN_NETWORK_BUILDER_H */
