#include "brain_csv_utils.h"
#include "brain_logging_utils.h"
#include "brain_memory_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void
load_csv_file(const BrainString csv,
              const BrainString tokenizer,
              CsvLineCbk cbk,
              void* data)
{
    BRAIN_INPUT(load_csv_file)

    if (BRAIN_ALLOCATED(csv)        &&
        BRAIN_ALLOCATED(tokenizer)  &&
        BRAIN_ALLOCATED(cbk)        &&
        BRAIN_ALLOCATED(data))
    {
        // opening the file for reading
        FILE *repository = fopen(csv, "r");

        if (BRAIN_ALLOCATED(repository))
        {
            BrainChar* line = NULL;
            size_t     len  = 0;

            /****************************************************************/
            /**                 Browse the repository file                 **/
            /****************************************************************/
            while (getline(&line, &len, repository) != -1)
            {
                if ((len > 0) && (line != NULL))
                {
                    cbk(line, data);
                }
            }

            fclose(repository);
        }
        else
        {
            BRAIN_CRITICAL("Unable to open %s for reading\n", csv);
        }
    }
    else
    {
        BRAIN_CRITICAL("Unable to parse csv repository because file is invalid");
    }

    BRAIN_OUTPUT(load_csv_file)
}
