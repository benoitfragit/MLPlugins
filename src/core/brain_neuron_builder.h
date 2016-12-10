#ifndef BRAIN_NEURON_BUILDER_H
#define BRAIN_NEURON_BUILDER_H

#include "brain_xml_utils.h"
#include "brain_logging_utils.h"
#include "brain_activation.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

BrainNeuron new_neuron_from_context    (Context context, BrainDouble* out, BrainWeight weighted_deltas);
void        delete_neuron              (BrainNeuron neuron);
void        update_neuron              (BrainNeuron neuron);
void        activate_neuron            (BrainNeuron neuron);
void        dump_neuron                (BrainNeuron neuron, const BrainInt layer_idx, const BrainInt neuron_idx, FILE* file);
void        set_neuron_weight          (BrainNeuron neuron, const BrainInt index, const BrainDouble weight);
void        set_neuron_input           (BrainNeuron neuron, const BrainInt number_of_inputs, const BrainSignal in);
void        set_neuron_delta           (BrainNeuron neuron, const BrainDouble delta);
BrainDouble get_neuron_input           (BrainNeuron neuron, const BrainInt input_index);
BrainDouble get_neuron_output          (BrainNeuron neuron);
BrainDouble get_neuron_weight          (BrainNeuron neuron, const BrainInt weight_index);
BrainDouble get_neuron_bias            (BrainNeuron neuron);
BrainInt    get_neuron_number_of_inputs(BrainNeuron neuron);

#endif /* BRAIN_NEURON_BUILDER_H */
