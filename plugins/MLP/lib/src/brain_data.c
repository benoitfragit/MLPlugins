#include "brain_data.h"
#include "brain_logging_utils.h"
#include "brain_random_utils.h"
#include "brain_math_utils.h"
#include "brain_memory_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TRAINING_DATASET_RATIO 0.80

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
    BrainSignal  _means;    /*!< The means signal       */
    BrainSignal  _sigmas;   /*!< Ths variance signal    */
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
    BrainBool   _is_labelled;      /*!< activate the labelling algo   */
    BrainBool   _is_splitted;      /*!< data are splitted             */
    BrainBool   _is_normalized;    /*!< data are normalized           */
} Data;

static void
parse_csv_repository(BrainData       data,
                     BrainString     repository_path,
                     BrainString     tokenizer)
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

            if (data->_is_labelled)
            {
                // Assume that all Label names are in the last column
                number_of_fields = input_length + 1;
            }

            // Dynamic allocation of both training and evaluaing dataset
            data->_training._children   = 0;
            data->_evaluating._children = 0;
            BRAIN_NEW(data->_evaluating._means,   BrainReal, input_length);
            BRAIN_NEW(data->_evaluating._sigmas,  BrainReal, input_length);
            BRAIN_NEW(data->_training._means,     BrainReal, input_length);
            BRAIN_NEW(data->_training._sigmas,    BrainReal, input_length);
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
                    if (data->_is_splitted &&
                        (BRAIN_RAND_UNIT < TRAINING_DATASET_RATIO))
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

                        if (k < input_length)
                        {
                            value = &((dataset->_input[dataset->_children - 1])[k]);
                        }
                        else
                        {
                            if (!data->_is_labelled)
                            {
                                kp -= input_length;

                                if (kp < output_length)
                                {
                                    value = &((dataset->_output[dataset->_children - 1])[kp]);
                                }
                            }
                        }

                        if (BRAIN_ALLOCATED(buffer))
                        {
                            if ((k < input_length) || !data->_is_labelled)
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
                                if (input_length <= k)
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

                            buffer = strtok(NULL, tokenizer);
                            ++k;
                        }
                    }
                }
            }

            fclose(repository);

            /**********************************************************/
            /**               Normalizing inputs data                **/
            /**********************************************************/
            if (data->_is_normalized)
            {
                if (data->_is_splitted)
                {
                    GaussianNormalization(data->_training._input,
                                          data->_training._means,
                                          data->_training._sigmas,
                                          data->_training._children,
                                          data->_input_length);
                    BRAIN_COPY(data->_training._means,  data->_evaluating._means, BrainReal, data->_input_length);
                    BRAIN_COPY(data->_training._sigmas, data->_evaluating._sigmas, BrainReal, data->_input_length);
                }
                else
                {
                    GaussianNormalization(data->_evaluating._input,
                                          data->_evaluating._means,
                                          data->_evaluating._sigmas,
                                          data->_evaluating._children,
                                          data->_input_length);
                }
            }
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

BrainData
new_data(BrainString repository_path,
         BrainString tokenizer,
         const BrainUint input_length,
         const BrainUint output_length,
         const BrainBool is_splitted,
         const BrainBool is_labedelled,
         const BrainBool is_normalized)
{
    BrainData _data = NULL;

    if (repository_path
    &&  tokenizer)
    {
        BRAIN_NEW(_data, Data, 1);
        _data->_input_length  = input_length;
        _data->_output_length = output_length;
        _data->_labels_length = 0;
        _data->_is_labelled   = is_labedelled;
        _data->_is_splitted   = is_splitted;
        _data->_is_normalized = is_normalized;

        parse_csv_repository(_data, repository_path, tokenizer);
    }

    return _data;
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
        BRAIN_DELETE(data->_evaluating._means);
        BRAIN_DELETE(data->_evaluating._sigmas);
        BRAIN_DELETE(data->_evaluating._input);
        BRAIN_DELETE(data->_evaluating._output);
        BRAIN_DELETE(data->_training._input);
        BRAIN_DELETE(data->_training._output);
        BRAIN_DELETE(data->_training._means);
        BRAIN_DELETE(data->_training._sigmas);
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
get_evaluating_sigmas(const BrainData data)
{
    BrainSignal ret = NULL;
    if (BRAIN_ALLOCATED(data))
    {
        ret = data->_evaluating._sigmas;
    }

    return ret;
}

BrainSignal
get_evaluating_means(const BrainData data)
{
    BrainSignal ret = NULL;
    if (BRAIN_ALLOCATED(data))
    {
        ret = data->_evaluating._means;
    }

    return ret;
}

BrainSignal
get_training_sigmas(const BrainData data)
{
    BrainSignal ret = NULL;
    if (BRAIN_ALLOCATED(data))
    {
        ret = data->_training._sigmas;
    }

    return ret;
}

BrainSignal
get_training_means(const BrainData data)
{
    BrainSignal ret = NULL;
    if (BRAIN_ALLOCATED(data))
    {
        ret = data->_training._means;
    }

    return ret;
}
