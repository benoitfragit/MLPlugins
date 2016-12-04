#ifndef BRAIN_NEURON_BUILDER_H
#define BRAIN_NEURON_BUILDER_H

#include "brain_xml_utils.h"
#include "brain_logging_utils.h"
#include "brain_activation.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

BrainNeuron new_neuron_from_context    (Context context);
void        delete_neuron              (BrainNeuron neuron);
void        update_neuron              (BrainNeuron neuron);
void        activate_neuron            (BrainNeuron neuron);
void        propagate_neuron           (BrainNeuron neuron, const BrainDouble out, const BrainInt input_index);
void        append_neuron_delta        (BrainNeuron neuron, const BrainDouble delta);
void        dump_neuron                (BrainNeuron neuron, const BrainInt layer_idx, const BrainInt neuron_idx, FILE* file);
void        set_neuron_weight          (BrainNeuron neuron, const BrainInt index, const BrainDouble weight);
void        set_neuron_input           (BrainNeuron neuron, const BrainInt number_of_inputs, const BrainDouble* in);
BrainDouble get_neuron_weighted_delta  (BrainNeuron neuron, const BrainInt index);
BrainDouble get_neuron_input           (BrainNeuron neuron, const BrainInt input_index);
BrainDouble get_neuron_output          (BrainNeuron neuron);
BrainDouble get_neuron_weight          (BrainNeuron neuron, const BrainInt weight_index);
BrainInt    get_neuron_number_of_inputs(BrainNeuron neuron);

#endif /* BRAIN_NEURON_BUILDER_H */
