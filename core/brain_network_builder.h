#ifndef BRAIN_NETWORK_BUILDER_H
#define BRAIN_NETWORK_BUILDER_H

#include "brain_layer_builder.h"
#include "brain_synapse_builder.h"

typedef struct Network
{
    Layer_t   *_layers;
    Synapse_t *_synapses;
    double    *_output;

	int _is_trained;

    int _number_of_synapse;
    int _number_of_layer;
} Network;

Network*  new_network_from_context(Context context);
void      delete_network(Network *network);
Layer_t   layer(Network* network, const int layer_index);
Synapse_t synapse(Network* network, const int layer_id, const int neuron_id);
const double* getoutput(const Network* network);
void set_network_input(Network* network, const int number_of_input, const double *in);
void dump_network(const Network* network, const char* filename);

#endif /* BRAIN_NETWORK_BUILDER_H */
