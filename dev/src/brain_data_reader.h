/**
 * \file brain_data_reader.h
 * \brief Define the API to read training data from an XML file
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to read training data from an XML context
 */
#ifndef BRAIN_DATA_READER_H
#define BRAIN_DATA_READER_H

#include "brain_types.h"


/**
 * \fn BrainData new_data(const BrainUint number_of_signals,
 *                        const BrainUint signal_length,
 *                        const BrainUint observation_length)
 * \brief Load training data
 *
 * \param number_of_signals Number of signal in this dataset
 * \param signal_length Size of input signal
 * \param observation_length Size of output signal
 * \return a newly allocated BrainData
 */
BrainData
new_data(const BrainUint number_of_signals,
         const BrainUint signal_length,
         const BrainUint observation_length);
/**
 * \fn void append_data(BrainData data,
 *                      const BrainUint index,
 *                      BrainSignal input,
 *                      BrainSignal output)
 * \brief append training data
 *
 * \param data   Dataset
 * \param index  Index of the training sample
 * \param input  Input signal
 * \param output Output signal
 */
void append_data(BrainData data,
                 const BrainUint index,
                 BrainSignal input,
                 BrainSignal output);
/**
 * \fn void delete_data(BrainData data)
 * \brief free all memory owned by a BrainData
 *
 * \param data BrainData to free
 */
void        delete_data                 (BrainData data);
/**
 * \fn BrainSignal get_data_input(const BrainData data,
 *                                const BrainUint index)
 * \brief get on input signal from the BrainData
 *
 * \param data The BrainData struct
 * \param index the input signal index
 *
 * \return an input BrainSignal
 */
BrainSignal get_data_input              (const BrainData data,
                                         const BrainUint index);
/**
 * \fn BrainSignal get_data_output(const BrainData data,
 *                                 const BrainUint index)
 * \brief get an output signal from the BrainData
 *
 * \param data The BrainData struct
 * \param index the output signal index
 *
 * \return an input BrainSignal
 */
BrainSignal get_data_output             (const BrainData data,
                                         const BrainUint index);
/**
 * \fn BrainUint get_data_random_subset_index(const BrainData data)
 * \brief get a random input index
 *
 * \param data The BrainData struct
 * \return a random index
 */
BrainUint   get_data_random_subset_index(const BrainData data);
/**
 * \fn BrainUint get_data_input_length(const BrainData data)
 * \brief get input signal length
 *
 * \param data The BrainData struct
 * \return input signal length
 */
BrainUint   get_data_input_length       (const BrainData data);
/**
 * \fn BrainUint get_data_output_length(const BrainData data)
 * \brief get output signal length
 *
 * \param data The BrainData struct
 * \return output signal length
 */
BrainUint   get_data_output_length      (const BrainData data);
/**
 * \fn BrainUint get_data_subset_length(const BrainData data)
 * \brief get the number of signal in the training set
 *
 * \param data The BrainData struct
 * \return number of signals in the training set
 */
BrainUint   get_data_subset_length(const BrainData data);
#endif /* BRAIN_DATA_READER_H */
