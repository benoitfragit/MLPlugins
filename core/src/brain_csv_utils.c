#include "brain_csv_utils.h"
#include "brain_logging_utils.h"
#include "brain_memory_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct CsvReader
{
    BrainString     _path;
    BrainString     _tokenizer;
    BrainUint       _number_of_fields;
    BrainDataFormat _format;
    BrainBool       _is_labelled;
} CsvReader;

BrainCsvReader
new_csv_reader( BrainString path,
                BrainString tokenizer,
                const BrainUint number_of_fields,
                const BrainDataFormat format,
                const BrainBool is_labelled)
{
    BRAIN_INPUT(new_csv_reader)

    BrainCsvReader reader = NULL;

    if (BRAIN_ALLOCATED(path) &&
        BRAIN_ALLOCATED(tokenizer))
    {
        BRAIN_NEW(reader, CsvReader, 1);

        // This is pointer copy
        reader->_path               = path;
        reader->_tokenizer          = tokenizer;
        reader->_number_of_fields   = number_of_fields;
        reader->_is_labelled        = is_labelled;
        reader->_format             = format;
    }

    BRAIN_OUTPUT(new_csv_reader)

    return reader;
}

void
delete_csv_reader(BrainCsvReader reader)
{
    BRAIN_INPUT(delete_csv_reader)

    if (reader)
    {
        BRAIN_DELETE(reader);
    }

    BRAIN_OUTPUT(delete_csv_reader)
}

void
csv_reader_load(BrainCsvReader reader,
                CsvLineCbk cbk,
                void* data)
{
    BRAIN_INPUT(csv_reader_load)

    if (BRAIN_ALLOCATED(reader)     &&
        BRAIN_ALLOCATED(reader->_tokenizer)  &&
        BRAIN_ALLOCATED(cbk)        &&
        BRAIN_ALLOCATED(data)       )
    {
        BrainChar* line = NULL;
        size_t     len  = 0;

        FILE* file = fopen(reader->_path, "r");

        if (BRAIN_ALLOCATED(file))
        {
            /****************************************************************/
            /**                 Browse the repository file                 **/
            /****************************************************************/
            while (getline(&line, &len, file) != -1)
            {
                if ((len > 0) &&
                    (line != NULL))
                {
                    BrainReal* signal = NULL;
                    BrainChar* buffer = strtok(line, reader->_tokenizer);
                    BrainUint k = 0;

                    BRAIN_NEW(signal, BrainReal, reader->_number_of_fields);

                    if (BRAIN_ALLOCATED(buffer))
                    {
                        BrainChar* label = NULL;
                        const BrainUint length = strlen(buffer);

                        if (reader->_is_labelled)
                        {
                            BRAIN_NEW(label, BrainChar, length);
                        }

                        while(k < reader->_number_of_fields)
                        {
                            if (k == 0 && reader->_format == Format_OutputFirst && reader->_is_labelled)
                            {
                                // copy the buffer into the label
                                buffer[length - 1] = '\0';
                                label = strcpy(label, buffer);
                            }
                            else
                            {
#if BRAIN_ENABLE_DOUBLE_PRECISION
                                sscanf(buffer, "%lf", &(signal[k]));
#else
                                sscanf(buffer, "%f", &(signal[k]));
#endif
                            }
                            buffer = strtok(NULL, reader->_tokenizer);

                            ++k;
                        }

                        if (reader->_format == Format_InputFirst && reader->_is_labelled)
                        {
                            // copy the buffer into the label
                            buffer[length - 1] = '\0';
                            label = strcpy(label, buffer);
                        }

                        // Call the callback function
                        cbk(data, label, signal);

                        BRAIN_DELETE(signal);
                    }
                }
            }

            fclose(file);
        }
    }
    else
    {
        BRAIN_CRITICAL("Unable to open %s for reading\n", reader->_path);
    }

    BRAIN_OUTPUT(csv_reader_load)
}
