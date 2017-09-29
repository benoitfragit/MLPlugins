/**
 * \file brain_data.h
 * \brief Define the API to create a data tree
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to store training and evaluating data from an XML context
 */
#ifndef BRAIN_DATA_H
#define BRAIN_DATA_H

#include "brain_types.h"

/**
 * \fn BrainData new_data(const BrainUint input_length, const BrainUint output_length, const BrainBool is_data_splitted)
 * \brief create a data list
 *
 * \param input_length size of all input signal
 * \param output_length size of all output signal
 * \param is_data_splitted automatically split data into 2 subsets
 * \return a BrainData
 */
BrainData   new_data(const BrainUint input_length, const BrainUint output_length, const BrainBool is_data_splitted);
/**
 * \fn BrainNode get_training_node(const BrainData data);
 * \brief get the training root node
 *
 * \param data a BrainData
 * \return a BrainNode
 */
BrainNode   get_training_node(const BrainData data);
/**
 * \fn BrainNode get_evaluating_node(const BrainData data);
 * \brief get the evaluating root node
 *
 * \param data a BrainData
 * \return a BrainNode
 */
BrainNode   get_evaluating_node(const BrainData data);
/**
 * \fn BrainNode get_next_node(const BrainNode node);
 * \brief get the next node
 *
 * \param node a BrainNode
 * \return a BrainNode
 */
BrainNode   get_next_node(const BrainNode node);
/**
 * \fn BrainSignal get_node_input_signal(const BrainNode node);
 * \brief get the input signal
 *
 * \param node a BrainNode
 * \return a BrainSignal
 */
BrainSignal get_node_input_signal(const BrainNode node);
/**
 * \fn BrainSignal get_node_output_signal(const BrainNode node);
 * \brief get the output signal
 *
 * \param node a BrainNode
 * \return a BrainSignal
 */
BrainSignal get_node_output_signal(const BrainNode node);
/**
 * \fn void delete_data(const BrainData data);
 * \brief delete a data
 *
 * \param data A BrainData
 */
void        delete_data(BrainData data);
/**
 * \fn void new_node(BrainData data, BrainSignal input, BrainSignal output)
 * \brief create a new node
 *
 * \param data a BrainData
 * \param input a BrainSignal
 * \param output a BrainSignal
 */
void        new_node(BrainData data, BrainSignal input, BrainSignal output);
/**
 * \fn BrainUint get_input_signal_length(const BrainData data)
 * \brief get all input signal size
 *
 * \param data a BrainData
 * \return size of all input signal
 */
BrainUint get_input_signal_length(const BrainData data);
/**
 * \fn BrainUint get_output_signal_length(const BrainData data)
 * \brief get all output signal size
 *
 * \param data a BrainData
 * \return size of all output signal
 */
BrainUint get_output_signal_length(const BrainData data);
/**
 * \fn BrainUint get_node_children(const BrainData data)
 * \brief get the number of children for this node including this node
 *
 * \param data a BrainData
 * \return number of node
 */
BrainUint get_node_children(const BrainData data);
/**
 * \fn void preprocess(BrainData data)
 * \brief preprocess data tree by using centering and normalization
 *
 * \param data a BrainData
 */
void preprocess(BrainData data);
#endif /* BRAIN_DATA_H */
