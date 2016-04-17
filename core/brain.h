#ifndef BRAIN_H
#define BRAIN_H

#include "brain_type.h"

Network* createNetwork(const int m, int *n, int **o);
void deleteNetwork(Network *network);
int connect(Network* network,
               const int first_layer,
               const int second_layer,
               const int first_neuron,
               const int second_neuron,
               const int input_index);
const double* getOutput(const Network* networkt);
void setInput(Network* network, const int number_of_input, const double *in);
void feed(Network *network);
double forward(Network *network, const int number_of_output, const double *desired);

#endif /* BRAIN_H */
