#ifndef BRAIN_LAYER_BUILDER_H
#define BRAIN_LAYER_BUILDER_H

#include "brain_neuron_builder.h"
#include "brain_xml_utils.h"

typedef struct Layer
{
    Neuron **_neurons;
    int _number_of_neuron;
    int _id;
} Layer;

Layer*  new_layer_from_context(Context context);
Neuron* neuron(Layer* layer, const int index);
void    delete_layer(Layer *layer);
void    set_layer_input(Layer* layer, const int number_of_inputs, const double* in);

#endif /* BRAIN_LAYER_BUILDER_H */
