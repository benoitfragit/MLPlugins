#ifndef BRAIN_CSV_UTILS_H
#define BRAIN_CSV_UTILS_H

#include "brain_core_types.h"

typedef BrainBool (*CsvLineCbk)(BrainString line, void* data);


void load_csv_file( const BrainString csv,
                    const BrainString tokenizer,
                    CsvLineCbk cbk,
                    void* data);

#endif /* BRAIN_CSV_UTILS_H */
