#ifndef BRAIN_TYPE_H
#define BRAIN_TYPE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

typedef struct Connection
{
    int _input_neuron;
    int _input_layer;
    int _output_neuron;
    int _output_layer;
    int _output_input;
} Connection;

typedef struct NetworkDescriptor
{
    Connection* _connections;
    int   _number_of_connections;
    int   _number_of_layers;
    int*  _number_of_neuron_per_layers;
    int** _number_of_input_per_neuron;
} NetworkDescriptor;

typedef struct Neuron
{
    double *_in;
    double *_w;
    double _out;
    double _learning_rate;
    double _inertial_factor;
    double _delta;
    double *_correction;

    int _number_of_input;
} Neuron;

typedef struct Synapse
{
    Neuron*  _neuron;
    Neuron** _connected_neurons;

    int _number_of_connected_neuron;
} Synapse;


typedef struct Layer
{
    Neuron **_neurons;
    int _number_of_neuron;
} Layer;

typedef struct Network
{
    Layer **_layers;
    Synapse **_synapses;
    double* _output;

    int _number_of_synapse;
    int _number_of_layer;
} Network;

#endif /* BRAIN_TYPE_H */