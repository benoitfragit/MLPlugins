/**
 * \file brain_tree.h
 * \brief Define the API to create a data tree
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to store training and evaluating data from an XML context
 */
#ifndef BRAIN_TREE_H
#define BRAIN_TREE_H

#include "brain_types.h"

/**
 * \fn BrainTree new_tree(const BrainUint input_length, const BrainUint output_length, const BrainBool is_data_splitted)
 * \brief create a data tree
 *
 * \param input_length size of all input signal
 * \param output_length size of all output signal
 * \param is_data_splitted automatically split data into 2 subsets
 * \return a BrainTree
 */
BrainTree   new_tree(const BrainUint input_length, const BrainUint output_length, const BrainBool is_data_splitted);
/**
 * \fn BrainNode get_training_node(const BrainTree tree);
 * \brief get the training root node
 *
 * \param tree a BrainTree
 * \return a BrainNode
 */
BrainNode   get_training_node(const BrainTree tree);
/**
 * \fn BrainNode get_evaluating_node(const BrainTree tree);
 * \brief get the evaluating root node
 *
 * \param tree a BrainTree
 * \return a BrainNode
 */
BrainNode   get_evaluating_node(const BrainTree tree);
/**
 * \fn BrainNode get_left_node(const BrainNode node);
 * \brief get the left node
 *
 * \param node a BrainNode
 * \return a BrainNode
 */
BrainNode   get_left_node(const BrainNode node);
/**
 * \fn BrainNode get_right_node(const BrainNode node);
 * \brief get the right node
 *
 * \param node a BrainNode
 * \return a BrainNode
 */
BrainNode   get_right_node(const BrainNode node);
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
 * \fn void delete_tree(const BrainTree tree);
 * \brief delete a tree
 *
 * \param tree A BrainTree
 */
void        delete_tree(BrainTree tree);
/**
 * \fn void new_node(BrainTree tree, BrainSignal input, BrainSignal output)
 * \brief create a new node
 *
 * \param tree a BrainTree
 * \param input a BrainSignal
 * \param output a BrainSignal
 */
void        new_node(BrainTree tree, BrainSignal input, BrainSignal output);
/**
 * \fn BrainUint get_input_signal_length(const BrainTree tree)
 * \brief get all input signal size
 *
 * \param tree a BrainTree
 * \return size of all input signal
 */
BrainUint get_input_signal_length(const BrainTree tree);
/**
 * \fn BrainUint get_output_signal_length(const BrainTree tree)
 * \brief get all output signal size
 *
 * \param tree a BrainTree
 * \return size of all output signal
 */
BrainUint get_output_signal_length(const BrainTree tree);
/**
 * \fn BrainUint get_node_children(const BrainNode node)
 * \brief get the number of children for this node including this node
 *
 * \param node a BrainNode
 * \return number of node
 */
BrainUint get_node_children(const BrainNode node);
/**
 * \fn void preprocess(BrainTree tree)
 * \brief preprocess data tree by using centering and normalization
 * 
 * \param tree a BrainTree
 */
void preprocess(BrainTree tree);
#endif /* BRAIN_TREE_H */
