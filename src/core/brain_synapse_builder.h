#ifndef BRAIN_SYNAPSE_BUILDER_H
#define BRAIN_SYNAPSE_BUILDER_H

#include "brain_xml_utils.h"
#include "brain_neuron_builder.h"

typedef struct Synapse* Synapse_t;

Synapse_t new_synapse();
void      delete_synapse                 (Synapse_t synapse);
void      activate_synapse               (Synapse_t synapse);
void      backpropagate_synapse          (Synapse_t synapse);
void      set_synapse_input_neuron       (Synapse_t synapse, Neuron_t neuron);
void      set_synapse_output_neuron      (Synapse_t synapse, Neuron_t neuron);
void      set_synapse_input_index        (Synapse_t synapse, const int input_index);
void      set_synapse_input_layer_index  (Synapse_t synapse, const int input_layer_index);
void      set_synapse_output_layer_index (Synapse_t synapse, const int output_layer_index);
void      set_synapse_input_neuron_index (Synapse_t synapse, const int input_neuron_index);
void      set_synapse_output_neuron_index(Synapse_t synapse, const int output_neuron_index);
int       is_synapse_valid               (Synapse_t synapse);
int       get_synapse_input_layer_index  (const Synapse_t synapse);
int       get_synapse_output_layer_index (const Synapse_t synapse);
int       get_synapse_input_neuron_index (const Synapse_t synapse);
int       get_synapse_output_neuron_index(const Synapse_t synapse);
int       get_synapse_input_index        (const Synapse_t synapse);
Neuron_t  get_synapse_input_neuron       (const Synapse_t synapse);
Neuron_t  get_synapse_output_neuron      (const Synapse_t synapse);

#endif /* BRAIN_SYNAPSE_BUILDER_H */
