#ifndef BRAIN_LAYER_BUILDER_H
#define BRAIN_LAYER_BUILDER_H

#include "brain_neuron_builder.h"
#include "brain_xml_utils.h"

typedef struct Layer* Layer_t;

Layer_t  new_layer_from_context    (Context context);
Neuron_t get_layer_neuron          (Layer_t layer, const int index);
int      get_layer_number_of_neuron(Layer_t layer);
void     delete_layer              (Layer_t layer);
void     set_layer_input           (Layer_t layer, const int number_of_inputs, const double* in);
void     dump_layer                (Layer_t layer, const int layer_idx, FILE* file);
void     update_layer_weight       (Layer_t layer);


#endif /* BRAIN_LAYER_BUILDER_H */