#ifndef BRAIN_H
#define BRAIN_H

#include "brain_network_builder.h"
#include "brain_data_reader.h"

void   feedforward  (Network_t network);
double backpropagate(Network_t network, const int number_of_output, const double *desired);
void   train        (Network_t network, const Data_t data, const double target_error, const int max_iter);

#endif /* BRAIN_H */
