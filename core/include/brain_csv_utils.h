#ifndef BRAIN_CSV_UTILS_H
#define BRAIN_CSV_UTILS_H

#include "brain_core_types.h"

typedef void (*CsvLineCbk)(void* data, BrainString label, const BrainReal* signal);

BrainCsvReader new_csv_reader(  BrainString path,
                                BrainString tokenizer,
                                const BrainUint number_of_fields,
                                const BrainDataFormat format,
                                const BrainBool is_labelled);

void delete_csv_reader(BrainCsvReader reader);

void csv_reader_load(BrainCsvReader reader, CsvLineCbk cbk, void* data);
#endif /* BRAIN_CSV_UTILS_H */
