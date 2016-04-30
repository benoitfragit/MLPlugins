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

typedef struct Neuron
{
    double* _in;
    double* _w;
    double  _out;
    double  _learning_rate;
    double  _inertial_factor;
    double  _delta;
    double* _correction;
    PtrFunc _activation;
    PtrFunc _derivative;
    int     _id;
    int     _number_of_input;
} Neuron;

Neuron* new_neuron_from_context(Context context);
void    delete_neuron(Neuron* neuron);
void    update(Neuron* neuron);
void    activate(Neuron* neuron);
double  input(Neuron* neuron, const int input_index);
double  weight(Neuron* neuron, const int weight_index);
void    set_neuron_input(Neuron* neuron, const int number_of_inputs, const double* in);
void    propagate(Neuron* neuron, const double out, const int input_index);
void    append_delta(Neuron* neuron, const double delta);
double  get_weighted_delta(Neuron *neuron, const int index);
void    dump_neuron(Neuron* neuron, FILE* file);

#endif /* BRAIN_NEURON_BUILDER_H */
