#ifndef BRAIN_LAYER_BUILDER_H
#define BRAIN_LAYER_BUILDER_H

#include "brain_neuron_builder.h"
#include "brain_xml_utils.h"

BrainLayer  new_layer_from_context    (Context context);
BrainNeuron get_layer_neuron          (const BrainLayer layer,
                                       const BrainUint index);
BrainUint   get_layer_number_of_neuron(const BrainLayer layer);
BrainSignal get_layer_output          (const BrainLayer layer);
BrainDouble get_layer_weighted_delta  (const BrainLayer layer,
                                       const BrainUint input_index);
BrainLayer  get_layer_next_layer      (const BrainLayer layer);
void        delete_layer              (BrainLayer layer);
void        set_layer_input           (BrainLayer layer,
                                       const BrainUint number_of_inputs,
                                       const BrainSignal in,
                                       const BrainBool use_dropbout,
                                       const BrainDouble dropout_percent);
void        dump_layer                (const BrainLayer layer,
                                       const BrainUint layer_idx,
                                       FILE* file);
void        reset_layer_delta         (BrainLayer layer);
void        set_layer_next_layer      (BrainLayer layer,
                                       BrainLayer next_layer);
#endif /* BRAIN_LAYER_BUILDER_H */
