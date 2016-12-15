#ifndef BRAIN_NEURON_BUILDER_H
#define BRAIN_NEURON_BUILDER_H

#include "brain_xml_utils.h"
#include "brain_logging_utils.h"
#include "brain_activation.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

BrainNeuron new_neuron_from_context    (Context           context,
                                        BrainDouble*      out,
                                        BrainWeight       weighted_deltas);
void        initialize_neuron_from_context(BrainNeuron    neuron,
                                           Context        context);
void        delete_neuron              (BrainNeuron       neuron);
void        activate_neuron            (BrainNeuron       neuron,
                                        const BrainDouble dropout_factor);
void        dump_neuron                (const BrainNeuron neuron,
                                        const BrainUint   layer_idx,
                                        const BrainUint   neuron_idx,
                                        FILE*             file);
void        set_neuron_weight          (BrainNeuron       neuron,
                                        const BrainUint   index,
                                        const BrainDouble weight);
void        set_neuron_input           (BrainNeuron       neuron,
                                        const BrainUint   number_of_inputs,
                                        const BrainSignal in,
                                        const BrainBool   use_dropout,
                                        const BrainDouble dropout_percent);
void        set_neuron_delta           (BrainNeuron       neuron,
                                        const BrainDouble delta);
void        set_neuron_bias            (BrainNeuron       neuron,
                                        const BrainDouble bias);
BrainDouble get_neuron_input           (const BrainNeuron neuron,
                                        const BrainUint   input_index);
BrainDouble get_neuron_output          (const BrainNeuron neuron);
BrainDouble get_neuron_weight          (const BrainNeuron neuron,
                                        const BrainUint   weight_index);
BrainDouble get_neuron_bias            (const BrainNeuron neuron);
BrainUint   get_neuron_number_of_inputs(const BrainNeuron neuron);

#endif /* BRAIN_NEURON_BUILDER_H */
