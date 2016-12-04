#ifndef BRAIN_SYNAPSE_BUILDER_H
#define BRAIN_SYNAPSE_BUILDER_H

#include "brain_xml_utils.h"
#include "brain_neuron_builder.h"

BrainSynapse new_synapse();
void delete_synapse                 (BrainSynapse synapse);
void activate_synapse               (BrainSynapse synapse);
void backpropagate_synapse          (BrainSynapse synapse);
void set_synapse_input_neuron       (BrainSynapse synapse, BrainNeuron neuron);
void set_synapse_output_neuron      (BrainSynapse synapse, BrainNeuron neuron);
void set_synapse_input_index        (BrainSynapse synapse, const BrainInt input_index);
void set_synapse_input_layer_index  (BrainSynapse synapse, const BrainInt input_layer_index);
void set_synapse_output_layer_index (BrainSynapse synapse, const BrainInt output_layer_index);
void set_synapse_input_neuron_index (BrainSynapse synapse, const BrainInt input_neuron_index);
void set_synapse_output_neuron_index(BrainSynapse synapse, const BrainInt output_neuron_index);
BrainInt get_synapse_input_layer_index  (const BrainSynapse synapse);
BrainInt get_synapse_output_layer_index (const BrainSynapse synapse);
BrainInt get_synapse_input_neuron_index (const BrainSynapse synapse);
BrainInt get_synapse_output_neuron_index(const BrainSynapse synapse);
BrainInt get_synapse_input_index        (const BrainSynapse synapse);
BrainBool is_synapse_valid              (BrainSynapse synapse);
BrainNeuron get_synapse_input_neuron    (const BrainSynapse synapse);
BrainNeuron get_synapse_output_neuron   (const BrainSynapse synapse);

#endif /* BRAIN_SYNAPSE_BUILDER_H */
