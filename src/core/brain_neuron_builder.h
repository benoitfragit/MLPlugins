#ifndef BRAIN_NEURON_BUILDER_H
#define BRAIN_NEURON_BUILDER_H

#include "brain_xml_utils.h"
#include "brain_logging_utils.h"
#include "brain_activation.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

typedef struct Neuron* Neuron_t;

Neuron_t new_neuron_from_context    (Context context);
void     delete_neuron              (Neuron_t neuron);
void     update_neuron              (Neuron_t neuron);
void     activate_neuron            (Neuron_t neuron);
void     propagate_neuron           (Neuron_t neuron, const double out, const int input_index);
void     append_neuron_delta        (Neuron_t neuron, const double delta);
void     dump_neuron                (Neuron_t neuron, const int layer_idx, const int neuron_idx, FILE* file);
double   get_neuron_input           (Neuron_t neuron, const int input_index);
double   get_neuron_output          (Neuron_t neuron);
double   get_neuron_weight          (Neuron_t neuron, const int weight_index);
void     set_neuron_weight          (Neuron_t neuron, const int index, const double weight);
void     set_neuron_input           (Neuron_t neuron, const int number_of_inputs, const double* in);
double   get_neuron_weighted_delta  (Neuron_t neuron, const int index);
int      get_neuron_number_of_inputs(Neuron_t neuron);

#endif /* BRAIN_NEURON_BUILDER_H */
