#ifndef BRAIN_SYNAPSE_BUILDER_H
#define BRAIN_SYNAPSE_BUILDER_H

#include "brain_xml_utils.h"

typedef struct Synapse* Synapse_t;

Synapse_t new_synapse_from_context(Context context);
void delete_synapse(Synapse_t synapse);
void dump_synapse(Synapse_t synapse, FILE* file);
int get_input_layer(Synapse_t synapse);
int get_input_neuron(Synapse_t synapse);
int get_output_layer(Synapse_t synapse);
int get_output_neuron(Synapse_t synapse);
int get_input_index(Synapse_t synapse);

#endif /* BRAIN_SYNAPSE_BUILDER_H */
