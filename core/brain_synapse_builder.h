#ifndef BRAIN_SYNAPSE_BUILDER_H
#define BRAIN_SYNAPSE_BUILDER_H

#include "brain_xml_utils.h"

typedef struct Synapse
{
    int _input_layer;
    int _output_layer;
    int _input_neuron;
    int _output_neuron;
    int _input_index;
} Synapse;

Synapse* new_synapse_from_context(Context context);
void delete_synapse(Synapse* synapse);
void dump_synapse(Synapse* synapse, FILE* file);


#endif /* BRAIN_SYNAPSE_BUILDER_H */
