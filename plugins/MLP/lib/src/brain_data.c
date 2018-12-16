#include "brain_data.h"
#include "brain_config.h"
#include "brain_logging_utils.h"
#include "brain_random_utils.h"
#include "brain_signal_utils.h"
#include "brain_memory_utils.h"
#include "brain_xml_utils.h"
#include "brain_enum_utils.h"
#include "brain_csv_utils.h"
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

typedef enum DataPreprocessing
{
    Preprocessing_GaussianNormalization,
    Preprocessing_MinMaxNormalization,
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
    "GaussianNormalization",
    "MinMaxNormalization"
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

typedef struct GaussianModel
{
    BrainSignal _means;
    BrainSignal _sigmas;
} GaussianModel;

typedef struct MinMaxModel
{
    BrainSignal _min;
    BrainSignal _max;
} MinMaxModel;

typedef union DataModel
{
    MinMaxModel     _minMaxModel;
    GaussianModel   _gaussianModel;
} BrainDataModel;
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
    BrainDataModel _model;         /*!< The brain data model          */
    BrainBool   _is_labelled;      /*!< Data are labelled             */
    BrainDataFormat _format;           /*!< Data format                   */
} Data;

static void
csv_line_callback(void* data, BrainString label, const BrainReal* signal)
{
    if (BRAIN_ALLOCATED(data) &&
        BRAIN_ALLOCATED(signal))
    {
        BrainData pData = (BrainData)data;
        const BrainUint input_length        = pData->_input_length;
        const BrainUint output_length       = pData->_output_length;
        /****************************************************************/
        /**              Randomly choose signal storage                **/
        /****************************************************************/
        Dataset* dataset = &(pData->_evaluating);
        if (BRAIN_RAND_UNIT < TRAINING_DATASET_RATIO)
        {
            dataset = &(pData->_training);
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
        /**                COPY THE LABEL AND SIGNAL                    */
        /****************************************************************/
        if (pData->_is_labelled)
        {
            if (BRAIN_ALLOCATED(label))
            {
                BrainUint i = 0;
                BrainBool found = BRAIN_FALSE;

                for (i = 0; i < pData->_labels_length; ++i)
                {
                    if (BRAIN_ALLOCATED(pData->_labels[i])
                    &&  !strcmp(pData->_labels[i], label))
                    {
                        found = BRAIN_TRUE;
                        dataset->_output[dataset->_children - 1][i] = 1.;
                        break;
                    }
                }

                if (!found)
                {
                    const BrainUint length = strlen(label);
                    ++pData->_labels_length;
                    BRAIN_RESIZE(pData->_labels, BrainChar*, pData->_labels_length);
                    BRAIN_NEW(pData->_labels[pData->_labels_length - 1], BrainChar, length);
                    pData->_labels[pData->_labels_length - 1] = strcpy(pData->_labels[pData->_labels_length - 1], label);
                    dataset->_output[dataset->_children - 1][pData->_labels_length - 1] = 1.;
                }

                BRAIN_COPY(signal, dataset->_input[dataset->_children - 1],BrainReal,input_length);
            }
        }
        else
        {
            /****************************************************************/
            /**                    Use CSV data                            **/
            /****************************************************************/
            switch(pData->_format)
            {
                case Format_InputFirst:
                {
                    BRAIN_COPY(signal,
                                dataset->_input[dataset->_children - 1],
                                BrainReal,
                                input_length);
                    BRAIN_COPY(signal + input_length,
                                dataset->_output[dataset->_children - 1],
                                BrainReal,
                                output_length);
                }
                    break;
                case Format_OutputFirst:
                {
                    BRAIN_COPY(signal,
                                dataset->_output[dataset->_children - 1],
                                BrainReal,
                                output_length);
                    BRAIN_COPY(signal + output_length,
                                dataset->_input[dataset->_children - 1],
                                BrainReal,
                                input_length);
                }
                    break;
                default:
                    break;
            }
        }
    }
}

static BrainData
new_data(BrainString repository_path,
         BrainString tokenizer,
         const BrainUint input_length,
         const BrainUint output_length,
         const DataParser parser,
         const BrainBool is_labedelled,
         const BrainDataFormat format,
         const BrainUint number_of_preprocessing,
         const DataPreprocessing* preprocessings)
{
    BrainData _data = NULL;

    if (repository_path)
    {
        BrainUint i = 0;

        BRAIN_NEW(_data, Data, 1);

        _data->_input_length    = input_length;
        _data->_output_length   = output_length;
        _data->_labels_length   = 0;
        _data->_is_labelled     = is_labedelled;
        _data->_format          = format;

        printf("Format: %d", format);


        switch (parser)
        {
            case Parser_CSV:
            {
                const BrainUint number_of_fields = _data->_is_labelled ? _data->_input_length: _data->_input_length + _data->_output_length;
                // Create a CSV reader
                BrainCsvReader reader = new_csv_reader(repository_path,
                                                       tokenizer,
                                                       number_of_fields,
                                                       _data->_format,
                                                       _data->_is_labelled);
                // Load the CSV file
                csv_reader_load(reader, csv_line_callback, _data);
                // Delete the CSV reader
                delete_csv_reader(reader);
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
                    GaussianModel* model = &(_data->_model._gaussianModel);

                    BRAIN_NEW(model->_means, BrainReal,  _data->_input_length);
                    BRAIN_NEW(model->_sigmas, BrainReal, _data->_input_length);

                    FindGaussianModel(_data->_training._input,
                                      model->_means,
                                      model->_sigmas,
                                      _data->_training._children,
                                      _data->_input_length);
                    ApplyGaussianModel(_data->_training._input,
                                       model->_means,
                                       model->_sigmas,
                                       _data->_training._children,
                                       _data->_input_length);
                    ApplyGaussianModel(_data->_evaluating._input,
                                       model->_means,
                                       model->_sigmas,
                                       _data->_evaluating._children,
                                       _data->_input_length);
                }
                    break;
                case Preprocessing_MinMaxNormalization:
                {
                    MinMaxModel* model = &(_data->_model._minMaxModel);

                    BRAIN_NEW(model->_min, BrainReal, _data->_input_length);
                    BRAIN_NEW(model->_max, BrainReal, _data->_input_length);

                    FindMinMaxModel(_data->_training._input,
                                    model->_min,
                                    model->_max,
                                    _data->_training._children,
                                    _data->_input_length);
                    ApplyMinMaxModel(_data->_training._input,
                                       model->_min,
                                       model->_max,
                                       _data->_training._children,
                                       _data->_input_length);
                    ApplyMinMaxModel(_data->_evaluating._input,
                                       model->_min,
                                       model->_max,
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

BrainData __BRAIN_VISIBLE__
new_data_from_context(BrainString filepath)
{
    BRAIN_INPUT(new_data_from_context);
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
                const BrainDataFormat format = get_enum_values(_formats, Format_First, Format_Last, buffer);
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

    BRAIN_OUTPUT(new_data_from_context);

    return data;
}

void __BRAIN_VISIBLE__
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
        BRAIN_DELETE(data->_evaluating._input);
        BRAIN_DELETE(data->_evaluating._output);
        BRAIN_DELETE(data->_training._input);
        BRAIN_DELETE(data->_training._output);
        BRAIN_DELETE(data->_model._minMaxModel._min);
        BRAIN_DELETE(data->_model._minMaxModel._max);
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
