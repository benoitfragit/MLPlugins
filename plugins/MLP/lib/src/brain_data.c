#include "brain_data.h"
#include "brain_config.h"
#include "brain_logging_utils.h"
#include "brain_random_utils.h"
#include "brain_math_utils.h"
#include "brain_memory_utils.h"
#include "brain_xml_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TRAINING_DATASET_RATIO 0.80

typedef enum DataParser
{
    Parser_CSV,
    Parser_Invalide,
    Parser_First = Parser_CSV,
    Parser_Last  = Parser_Invalide
} DataParser;

typedef enum DataFormat
{
    Format_InputFirst,
    Format_OutputFirst,
    Format_Invalide,
    Format_First = Format_InputFirst,
    Format_Last  = Format_Invalide
} DataFormat;

typedef enum DataPreprocessing
{
    Preprocessing_GaussianNormalization,
    Preprocessing_Invalide,
    Preprocessing_First = Preprocessing_GaussianNormalization,
    Preprocessing_Last  = Preprocessing_Invalide
} DataPreprocessing;

static BrainString _parsers[] = {
    "csv"
};
static BrainString _formats[] = {
    "InputFirst",
    "OutputFirst"
};
static BrainString _preprocessings[] = {
    "GaussianNormalization"
};

/**
 * \struct Dataset
 * \brief  Internal model for a Dataset
 *
 * All protected fields for a Dataset
 */
typedef struct Dataset
{
    BrainSignal* _input;    /*!< The input signal       */
    BrainSignal* _output;   /*!< The output signal      */
    BrainUint    _children; /*!< The number of children */
} Dataset;
/**
 * \struct Data
 * \brief  Internal model for a BrainData
 *
 * All protected fields for a BrainData
 */
typedef struct Data
{
    Dataset     _training;         /*!< This is the training node     */
    Dataset     _evaluating;       /*!< This is the evaluating node   */
    BrainUint   _input_length;     /*!< input signal length           */
    BrainUint   _output_length;    /*!< output signal length          */
    BrainUint   _labels_length;    /*!< output label length           */
    BrainChar** _labels;           /*!< output label if needed        */
    BrainSignal _means;            /*!< The means signal              */
    BrainSignal _sigmas;           /*!< Ths variance signal           */
} Data;

static BrainUint
get_enum_values(const BrainString* labels,
                const BrainUint first,
                const BrainUint last,
                BrainString val)
{
    BrainUint ret = last;
    if (BRAIN_ALLOCATED(labels) &&
        BRAIN_ALLOCATED(val))
    {
        BrainUint i = first;
        for (i = first; i < last; ++i)
        {
            if (BRAIN_ALLOCATED(labels[i]) &&
                !strcmp(labels[i], val))
            {
                ret = i;
                break;
            }
        }
    }
    return ret;
}

static void
parse_csv_repository(BrainData   data,
                     BrainString repository_path,
                     BrainString tokenizer,
                     const BrainBool  is_labelled,
                     const DataFormat format)
{
    BRAIN_INPUT(parse_csv_repository)

    if (BRAIN_ALLOCATED(data) &&
        BRAIN_ALLOCATED(repository_path) &&
        BRAIN_ALLOCATED(tokenizer))
    {
        // opening the file for reading
        FILE *repository = fopen(repository_path, "r");

        if (BRAIN_ALLOCATED(repository))
        {
            const BrainUint input_length    = data->_input_length;
            const BrainUint output_length   = data->_output_length;

            BrainChar* line = NULL;
            BrainUint  number_of_fields = input_length + output_length;
            BrainUint  k    = 0;
            size_t     len  = 0;

            if (is_labelled)
            {
                // Assume that all Label names are in the last column
                number_of_fields = input_length + 1;
            }

            // Dynamic allocation of both training and evaluaing dataset
            data->_training._children   = 0;
            data->_evaluating._children = 0;
            /****************************************************************/
            /**                 Browse the repository file                 **/
            /****************************************************************/
            while (getline(&line, &len, repository) != -1)
            {
                if ((len > 0) && (line != NULL))
                {
                    BrainChar* buffer = NULL;
                    k = 0;
                    /****************************************************************/
                    /**              Randomly choose signal storage                **/
                    /****************************************************************/
                    Dataset* dataset = &(data->_evaluating);
                    if (BRAIN_RAND_UNIT < TRAINING_DATASET_RATIO)
                    {
                        dataset = &(data->_training);
                    }
                    /****************************************************************/
                    /**                        Append new signals                  **/
                    /****************************************************************/
                    ++(dataset->_children);
                    BRAIN_RESIZE(dataset->_input, BrainSignal, dataset->_children);
                    BRAIN_RESIZE(dataset->_output, BrainSignal, dataset->_children);
                    BRAIN_NEW(dataset->_input[dataset->_children - 1], BrainReal, input_length);
                    BRAIN_NEW(dataset->_output[dataset->_children - 1], BrainReal, output_length);
                    /****************************************************************/
                    /**                        Parsing signals                     **/
                    /****************************************************************/
                    buffer = strtok(line, tokenizer);

                    while(k < number_of_fields)
                    {
                        BrainSignal value = NULL;
                        BrainUint kp = k;

                        switch(format)
                        {
                            case Format_InputFirst:
                            {
                                if (k < input_length)
                                {
                                    value = &((dataset->_input[dataset->_children - 1])[k]);
                                }
                                else
                                {
                                    kp -= input_length;

                                    if (!is_labelled && kp < output_length)
                                    {
                                        value = &((dataset->_output[dataset->_children - 1])[kp]);
                                    }
                                }
                            }
                                break;
                            case Format_OutputFirst:
                            {
                                if (k < (number_of_fields - input_length))
                                {
                                    if (!is_labelled)
                                    {
                                        value = &((dataset->_output[dataset->_children - 1])[k]);
                                    }
                                }
                                else
                                {
                                    kp -= number_of_fields - input_length;
                                    value = &((dataset->_input[dataset->_children - 1])[kp]);
                                }
                            }
                                break;
                            default:
                                break;
                        }

                        if (BRAIN_ALLOCATED(buffer))
                        {
                            if (BRAIN_ALLOCATED(value))
                            {
#if BRAIN_ENABLE_DOUBLE_PRECISION
                                sscanf(buffer, "%lf", value);
#else
                                sscanf(buffer, "%f", value);
#endif
                            }
                            else
                            {
                                /**************************************/
                                /**        AUTOMATIC LABELING        **/
                                /**************************************/
                                if (is_labelled)
                                {
                                    if (((format == Format_InputFirst) && (input_length <= k))
                                    ||  ((format == Format_OutputFirst) && (k < number_of_fields - input_length)))
                                    {
                                        BrainUint i = 0;
                                        BrainBool found = BRAIN_FALSE;
                                        BrainUint length = strlen(buffer);
                                        buffer[length - 1] = '\0';

                                        for (i = 0; i < data->_labels_length; ++i)
                                        {
                                            if (BRAIN_ALLOCATED(data->_labels[i])
                                            &&  !strcmp(data->_labels[i], buffer))
                                            {
                                                found = BRAIN_TRUE;
                                                dataset->_output[dataset->_children - 1][i] = 1.;
                                                break;
                                            }
                                        }

                                        if (!found)
                                        {
                                            ++data->_labels_length;
                                            BRAIN_RESIZE(data->_labels, BrainChar*, data->_labels_length);
                                            BRAIN_NEW(data->_labels[data->_labels_length - 1], BrainChar, length);
                                            data->_labels[data->_labels_length - 1] = strcpy(data->_labels[data->_labels_length - 1], buffer);
                                            dataset->_output[dataset->_children - 1][data->_labels_length - 1] = 1.;
                                        }
                                    }
                                }
                            }

                            buffer = strtok(NULL, tokenizer);
                            ++k;
                        }
                    }
                }
            }

            fclose(repository);
        }
        else
        {
            BRAIN_CRITICAL("Unable to open %s for reading\n", repository_path);
        }
    }
    else
    {
        BRAIN_CRITICAL("Unable to parse csv repository because file is invalid");
    }

    BRAIN_OUTPUT(parse_csv_repository);
}

static BrainData
new_data(BrainString repository_path,
         BrainString tokenizer,
         const BrainUint input_length,
         const BrainUint output_length,
         const DataParser parser,
         const BrainBool is_labedelled,
         const DataFormat format,
         const BrainUint number_of_preprocessing,
         const DataPreprocessing* preprocessings)
{
    BrainData _data = NULL;

    if (repository_path)
    {
        BrainUint i = 0;

        BRAIN_NEW(_data, Data, 1);

        _data->_input_length  = input_length;
        _data->_output_length = output_length;
        _data->_labels_length = 0;

        switch (parser)
        {
            case Parser_CSV:
            {
                parse_csv_repository(_data,
                                     repository_path,
                                     tokenizer,
                                     is_labedelled,
                                     format);
            }
                break;
            default:
                break;
        }

        for (i = 0; i < number_of_preprocessing; ++i)
        {
            switch(preprocessings[i])
            {
                case Preprocessing_GaussianNormalization:
                {
                    BRAIN_NEW(_data->_means, BrainReal, _data->_input_length);
                    BRAIN_NEW(_data->_sigmas, BrainReal, _data->_input_length);

                    FindGaussianModel(_data->_training._input,
                                      _data->_means,
                                      _data->_sigmas,
                                      _data->_training._children,
                                      _data->_input_length);
                    ApplyGaussianModel(_data->_training._input,
                                       _data->_means,
                                       _data->_sigmas,
                                       _data->_training._children,
                                       _data->_input_length);
                    ApplyGaussianModel(_data->_evaluating._input,
                                       _data->_means,
                                       _data->_sigmas,
                                       _data->_evaluating._children,
                                       _data->_input_length);
                }
                    break;
                default:
                    break;
            }
        }
    }

    return _data;
}

BrainData
new_data_from_context(BrainString filepath)
{
    BrainData data = NULL;

    if (BRAIN_ALLOCATED(filepath)
    &&  validate_with_xsd(filepath, DATA_XSD_FILE))
    {
        Document data_document = open_document(filepath);

        if (BRAIN_ALLOCATED(data_document))
        {
            Context context = get_root_node(data_document);

            if (BRAIN_ALLOCATED(context) &&
                is_node_with_name(context, "data"))
            {
                BrainChar* buffer = NULL;
                BrainChar* repository = NULL;
                BrainChar* tokenizer = NULL;
                DataPreprocessing* preprocessings =  NULL;
                BrainUint i = 0;

                repository = (BrainChar*)node_get_prop(context, "repository");
                const BrainUint input_length    = node_get_int(context, "input-length", 1);
                const BrainUint output_length   = node_get_int(context, "output-length", 1);
                buffer = (BrainChar *)node_get_prop(context, "format");
                const DataFormat format = get_enum_values(_formats, Format_First, Format_Last, buffer);
                BRAIN_DELETE(buffer);
                buffer = (BrainChar *)node_get_prop(context, "parser");
                const DataParser parser = get_enum_values(_parsers, Parser_First, Parser_Last, buffer);
                BRAIN_DELETE(buffer);
                if (parser == Parser_CSV)
                {
                    tokenizer = (BrainChar *)node_get_prop(context, "tokenizer");
                }

                const BrainBool labelled = node_get_bool(context, "labels", BRAIN_FALSE);
                const BrainUint number_of_preprocessing = get_number_of_node_with_name(context, "preprocess");

                BRAIN_NEW(preprocessings, DataPreprocessing, number_of_preprocessing);
                for (i = 0; i < number_of_preprocessing; ++i)
                {
                    Context preprocessings_context = get_node_with_name_and_index(context, "preprocess", i);
                    buffer = (BrainChar*)node_get_prop(preprocessings_context, "type");
                    preprocessings[i] = get_enum_values(_preprocessings, Preprocessing_First, Preprocessing_Last, buffer);
                    BRAIN_DELETE(buffer);
                }

                data = new_data(repository,
                                tokenizer,
                                input_length,
                                output_length,
                                parser,
                                labelled,
                                format,
                                number_of_preprocessing,
                                preprocessings);

                BRAIN_DELETE(preprocessings);
                BRAIN_DELETE(tokenizer);
                BRAIN_DELETE(repository);
            }
        }
    }
    else
    {
        BRAIN_CRITICAL("Impossible de valider le fichier:%s\n", filepath);
    }

    return data;
}

void
delete_data(BrainData data)
{
    if (BRAIN_ALLOCATED(data))
    {
        BrainUint k = 0;
        BrainUint m = data->_training._children;
        if (m < data->_evaluating._children)
        {
            m = data->_evaluating._children;

            if (m < data->_labels_length)
            {
                m = data->_labels_length;
            }
        }

        for (k = 0; k < m; ++k)
        {
            if (k < data->_evaluating._children)
            {
                BRAIN_DELETE(data->_evaluating._input[k]);
                BRAIN_DELETE(data->_evaluating._output[k]);
            }
            if (k < data->_training._children)
            {
                BRAIN_DELETE(data->_training._input[k]);
                BRAIN_DELETE(data->_training._output[k]);
            }
            if (k < data->_labels_length)
            {
                BRAIN_DELETE(data->_labels[k]);
            }
        }

        BRAIN_DELETE(data->_labels);
        BRAIN_DELETE(data->_means);
        BRAIN_DELETE(data->_sigmas);
        BRAIN_DELETE(data->_evaluating._input);
        BRAIN_DELETE(data->_evaluating._output);
        BRAIN_DELETE(data->_training._input);
        BRAIN_DELETE(data->_training._output);
        BRAIN_DELETE(data);
    }
}

BrainUint
get_number_of_evaluating_sample(const BrainData data)
{
    BrainUint ret = 0;

    if (BRAIN_ALLOCATED(data))
    {
        ret = data->_evaluating._children;
    }

    return ret;
}

BrainSignal
get_evaluating_input_signal(const BrainData data, const BrainUint index)
{
    BrainSignal ret = NULL;

    if (BRAIN_ALLOCATED(data)
    &&  (index < data->_evaluating._children))
    {
        ret = data->_evaluating._input[index];
    }

    return ret;
}

BrainSignal
get_evaluating_output_signal(const BrainData data, const BrainUint index)
{
    BrainSignal ret = NULL;

    if (BRAIN_ALLOCATED(data)
    &&  (index < data->_evaluating._children))
    {
        ret = data->_evaluating._output[index];
    }

    return ret;
}

BrainUint
get_number_of_training_sample(const BrainData data)
{
    BrainUint ret = 0;

    if (BRAIN_ALLOCATED(data))
    {
        ret = data->_training._children;
    }

    return ret;
}

BrainSignal
get_training_input_signal(const BrainData data, const BrainUint index)
{
    BrainSignal ret = NULL;

    if (BRAIN_ALLOCATED(data)
    &&  (index < data->_training._children))
    {
        ret = data->_training._input[index];
    }

    return ret;
}

BrainSignal
get_training_output_signal(const BrainData data, const BrainUint index)
{
    BrainSignal ret = NULL;

    if (BRAIN_ALLOCATED(data)
    &&  (index < data->_training._children))
    {
        ret = data->_training._output[index];
    }

    return ret;
}

BrainUint
get_input_signal_length(const BrainData data)
{
    if (BRAIN_ALLOCATED(data))
    {
        return data->_input_length;
    }

    return 0;
}

BrainUint
get_output_signal_length(const BrainData data)
{
    if (BRAIN_ALLOCATED(data))
    {
        return data->_output_length;
    }

    return 0;
}

BrainSignal
get_sigmas(const BrainData data)
{
    BrainSignal ret = NULL;
    if (BRAIN_ALLOCATED(data))
    {
        ret = data->_sigmas;
    }

    return ret;
}

BrainSignal
get_means(const BrainData data)
{
    BrainSignal ret = NULL;
    if (BRAIN_ALLOCATED(data))
    {
        ret = data->_means;
    }

    return ret;
}
