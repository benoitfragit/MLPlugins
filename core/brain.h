#ifndef BRAIN_H
#define BRAIN_H

#include "brain_network_builder.h"
#include "brain_data_reader.h"

/* -------------------------------*/
/**
 * @Synopsis
 *
 * Propate a Signal from the input
 * layer to the output layer
 *
 * @Param network
 */
/* ---------------------------------*/
void feedforward(Network_t network);

/* -------------------------------*/
/**
 * @Synopsis  
 *
 * Backpropagate the error from the
 * output layer to the input layer
 *
 * @Param network the NeuralNetwork
 * @Param number_of_output the size of the output vector
 * @Param desired The desired output
 *
 * @Returns The network quadratic error
 */
/* ---------------------------------*/
double backpropagate(Network_t network, const int number_of_output, const double *desired);

/* -------------------------------*/
/**
 * @Synopsis  
 *
 * Train a NeuralNetwork with backpropagation learning
 *
 * @Param network The network to train
 * @Param data    The data input signal and desired output
 * @Param target_error the target error 
 * @Param max_iter maximum number of iteration
 */
/* ---------------------------------*/
void train(Network_t network, const Data_t data, const double target_error, const int max_iter);

#endif /* BRAIN_H */
