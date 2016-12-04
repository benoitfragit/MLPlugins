#ifndef BRAIN_H
#define BRAIN_H

#include "brain_network_builder.h"
#include "brain_data_reader.h"

void feedforward(BrainNetwork network);
BrainDouble backpropagate(BrainNetwork network, const BrainInt number_of_output, const BrainDouble *desired);
BrainResult train(BrainNetwork network, const Data_t data, const BrainDouble target_error, const BrainInt max_iter);

#endif /* BRAIN_H */
