#ifndef BRAIN_H
#define BRAIN_H

#include "brain_network_builder.h"

void feedforward(Network *network);
double backpropagate(Network *network, const int number_of_output, const double *desired);

#endif /* BRAIN_H */
