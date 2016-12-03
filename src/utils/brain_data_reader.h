#ifndef BRAIN_DATA_READER_H
#define BRAIN_DATA_READER_H

#include "brain_xml_utils.h"

typedef double* Signal;
typedef double* Observation;

typedef struct Data* Data_t;

Data_t new_data_from_context(Context context);
void   delete_data(Data_t data);
const  Signal get_signal(Data_t data, const int index);
const  Observation get_observation(Data_t data, const int index);
int    get_next_random_subset_index(const int* subset,
                                     const int length,
                                     const int max_index);
int    get_subset_length(const Data_t data);
int    get_subset_index(const Data_t data, const int subset_index);
int    get_signal_length(const Data_t data);
int    get_observation_length(const Data_t data);
#endif /* BRAIN_DATA_READER_H */
