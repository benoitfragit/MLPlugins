#ifndef BRAIN_DATA_READER_H
#define BRAIN_DATA_READER_H

#include "brain_xml_utils.h"
#include "brain_types.h"

BrainData new_data_from_context(Context context);

void delete_data(BrainData data);

BrainSignal get_data_input(const BrainData data,
                           const BrainUint index);
BrainSignal get_data_output(const BrainData data,
                            const BrainUint index);
BrainUint get_data_random_subset_index(const BrainData data);
BrainUint get_data_input_length(const BrainData data);
BrainUint get_data_output_length(const BrainData data);
#endif /* BRAIN_DATA_READER_H */
