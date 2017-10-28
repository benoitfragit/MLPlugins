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
 * \fn BrainData new_data(BrainString repository_path, BrainString tokenizer, const BrainUint input_length, const BrainUint output_length, const BrainBool is_data_splitted)
 * \brief create a data list
 * \param repository_path give the path of the repository
 * \param tokenizer list of separators
 * \param input_length size of all input signal
 * \param output_length size of all output signal
 * \param is_data_splitted automatically split data into 2 subsets
 * \return a BrainData
 */
BrainData   new_data(BrainString repository_path, BrainString tokenizer, const BrainUint input_length, const BrainUint output_length, const BrainBool is_data_splitted);
/**
 * \fn BrainUint get_number_of_evaluating_sample(const BrainData data);
 * \brief get the number of trainng sample
 *
 * \param data a BrainData
 * \return a BrainUint
 */
BrainUint get_number_of_evaluating_sample(const BrainData data);
/**
 * \fn BrainSignal get_evaluating_input_signal(const BrainData data, const BrainUint index);
 * \brief get the input signal
 *
 * \param data a BrainData
 * \param index Index of the input evaluating signal
 * \return a BrainSignal
 */
BrainSignal get_evaluating_input_signal(const BrainData data, const BrainUint index);
/**
 * \fn BrainSignal get_evaluating_output_signal(const BrainData data, const BrainUint index);
 * \brief get the output signal
 *
 * \param data a BrainData
 * \param index Index of the output signal
 * \return a BrainSignal
 */
BrainSignal get_evaluating_output_signal(const BrainData data, const BrainUint index);
/**
 * \fn BrainUint get_number_of_training_sample(const BrainData data);
 * \brief get the number of trainng sample
 *
 * \param data a BrainData
 * \return a BrainUint
 */
BrainUint get_number_of_training_sample(const BrainData data);
/**
 * \fn BrainSignal get_training_input_signal(const BrainData data, const BrainUint index);
 * \brief get the input signal
 *
 * \param data a BrainData
 * \param index Index of the input training signal
 * \return a BrainSignal
 */
BrainSignal get_training_input_signal(const BrainData data, const BrainUint index);
/**
 * \fn BrainSignal get_training_output_signal(const BrainData data, const BrainUint index);
 * \brief get the output signal
 *
 * \param data a BrainData
 * \param index Index of the output signal
 * \return a BrainSignal
 */
BrainSignal get_training_output_signal(const BrainData data, const BrainUint index);
/**
 * \fn void delete_data(const BrainData data);
 * \brief delete a data
 *
 * \param data A BrainData
 */
void        delete_data(BrainData data);
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
 * \fn BrainSignal get_input_sigmas(const BrainData data)
 * \brief get input variances signal
 *
 * \param data a BrainData
 * \return variance vector
 */
BrainSignal get_input_sigmas(const BrainData data);
/**
 * \fn BrainSignal get_input_means(const BrainData data)
 * \brief get input means signal
 *
 * \param data a BrainData
 * \return means vector
 */
BrainSignal get_input_means(const BrainData data);

#endif /* BRAIN_DATA_H */
