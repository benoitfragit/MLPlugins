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

Neuron_t new_neuron_from_context(Context context);
void     delete_neuron(Neuron_t neuron);
void     update(Neuron_t neuron);
void     activate(Neuron_t neuron);
double   input(Neuron_t neuron, const int input_index);
double   output(Neuron_t neuron);
double   weight(Neuron_t neuron, const int weight_index);
void     set_neuron_weight(Neuron_t neuron, const int index, const double weight);
void     set_neuron_input(Neuron_t neuron, const int number_of_inputs, const double* in);
void     propagate(Neuron_t neuron, const double out, const int input_index);
void     append_delta(Neuron_t neuron, const double delta);
double   get_weighted_delta(Neuron_t neuron, const int index);
void     dump_neuron(Neuron_t neuron, const int layer_idx, const int neuron_idx, FILE* file);
int      get_neuron_id(Neuron_t neuron);
int      get_number_of_inputs(Neuron_t neuron);

#endif /* BRAIN_NEURON_BUILDER_H */
