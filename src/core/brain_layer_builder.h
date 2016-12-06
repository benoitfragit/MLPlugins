#ifndef BRAIN_LAYER_BUILDER_H
#define BRAIN_LAYER_BUILDER_H

#include "brain_neuron_builder.h"
#include "brain_xml_utils.h"

BrainLayer  new_layer_from_context    (Context context);
BrainNeuron get_layer_neuron          (BrainLayer layer, const BrainInt index);
BrainInt    get_layer_number_of_neuron(BrainLayer layer);
void        delete_layer              (BrainLayer layer);
void        set_layer_input           (BrainLayer layer, const BrainInt number_of_inputs, const BrainSignal in);
void        dump_layer                (BrainLayer layer, const BrainInt layer_idx, FILE* file);
void        update_layer_weight       (BrainLayer layer);


#endif /* BRAIN_LAYER_BUILDER_H */
