#include "brain_data.h"
#include "brain_logging_utils.h"
#include "brain_random_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TRAINING_DATASET_RATIO 0.55

/**
 * \struct Dataset
 * \brief  Internal model for a Dataset
 *
 * All protected fields for a Dataset
 */
typedef struct Dataset
{
    BrainSignal* _input;    /*!< The input signal */
    BrainSignal* _output;   /*!< The output signal */
    BrainUint   _children; /*!< The number of children */
} Dataset;

/**
 * \struct Data
 * \brief  Internal model for a BrainData
 *
 * All protected fields for a BrainData
 */
struct Data
{
    Dataset     _training;         /*!< This is the training node       */
    Dataset     _evaluating;       /*!< This is the evaluating node     */

    BrainUint   _input_length;     /*!< input signal length             */
    BrainUint   _output_length;    /*!< output signal length            */

    BrainSignal _means;            /*!< mean input vector               */
    BrainSignal _sigmas;           /*!< standerd deviation input vector */
} Data;

static void
parse_csv_repository(BrainData       data,
                     BrainString     repository_path,
                     BrainString     tokenizer,
                     const BrainBool is_data_splitted)
{
    if ((data            != NULL) &&
        (repository_path != NULL) &&
        (tokenizer       != NULL) )
    {
        // opening the file for reading
        FILE *repository = fopen(repository_path, "r");

        if (repository != NULL)
        {
            const BrainUint input_length = data->_input_length;
            const BrainUint output_length = data->_output_length;

            BrainUint  counter = 0;
            BrainChar* line   = NULL;
            BrainUint  j = 0;
            BrainUint  k = 0;
            BrainUint  m = 0;
            size_t     len    = 0;

            /****************************************************************/
            /**                Find the number of signal                   **/
            /****************************************************************/
            while (getline(&line, &len, repository) != -1)
            {
                if ((len > 0) && (line != NULL))
                {
                    ++counter;
                }
            }

            // Back to the file starting position
            rewind(repository);

            // Dynamic allocation of both training and evaluaing dataset
            data->_training._children   = 0;

            if (is_data_splitted)
            {
                data->_training._input  = (BrainSignal*)calloc(counter, sizeof(BrainSignal));
                data->_training._output = (BrainSignal*)calloc(counter, sizeof(BrainSignal));
            }

            data->_evaluating._input    = (BrainSignal*)calloc(counter, sizeof(BrainSignal));
            data->_evaluating._output   = (BrainSignal*)calloc(counter, sizeof(BrainSignal));
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
                    if (is_data_splitted ||
                        (BRAIN_RAND_UNIT <  TRAINING_DATASET_RATIO))
                    {
                        dataset = &(data->_training);
                    }

                    dataset->_input[dataset->_children]  = (BrainSignal)calloc(input_length, sizeof(BrainReal));
                    dataset->_output[dataset->_children] = (BrainSignal)calloc(output_length, sizeof(BrainReal));

                    /****************************************************************/
                    /**                        Parsing signals                     **/
                    /****************************************************************/
                    buffer = strtok(line, tokenizer);

                    while(k < input_length + output_length)
                    {
                        BrainSignal value = NULL;
                        BrainUint kp = k;

                        if (k < input_length)
                        {
                            value = &((dataset->_input[dataset->_children])[k]);
                        }
                        else
                        {
                            kp -= input_length;

                            if (kp < output_length)
                            {
                                value = &((dataset->_output[dataset->_children])[kp]);
                            }
                        }

                        if (buffer != NULL && value != NULL)
                        {
#if BRAIN_ENABLE_DOUBLE_PRECISION
                            sscanf(buffer, "%lf", value);
#else
                            sscanf(buffer, "%f", value);
#endif
                            buffer = strtok(NULL, tokenizer);

                            /****************************************************************/
                            /**           Directly compute cenering parameters             **/
                            /****************************************************************/
                            if (k < input_length)
                            {
                                data->_means[k]  += *value;
                                data->_sigmas[k] += (*value) * (*value);
                            }

                            ++k;
                        }
                    }

                    ++dataset->_children;
                    ++counter;
                }
            }

            fclose(repository);

            /****************************************************************/
            /**                 Normalizing inputs data                    **/
            /****************************************************************/
            m = data->_training._children;
            if (m < data->_evaluating._children)
            {
                m = data->_evaluating._children;
            }

            for (k = 0; k < input_length; ++k)
            {
                data->_means[k] /= (BrainReal)counter;
                data->_sigmas[k] = (data->_sigmas[k] / (BrainReal)counter) - data->_means[k] * data->_means[k];

                for (j = 0; j < m; ++j)
                {
                    if (j < data->_evaluating._children)
                    {
                        (data->_evaluating._input[j])[k] -= data->_means[k];
                        (data->_evaluating._input[j])[k] /= data->_sigmas[k];
                    }
                    if (j < data->_training._children)
                    {
                        (data->_training._input[j])[k] -= data->_means[k];
                        (data->_training._input[j])[k] /= data->_sigmas[k];
                    }
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
}

BrainData
new_data(BrainString repository_path,
         BrainString tokenizer,
         const BrainUint input_length,
         const BrainUint output_length,
         const BrainBool is_data_splitted)
{
    BrainData _data = NULL;

    if (repository_path && tokenizer)
    {
        _data = (BrainData)calloc(1, sizeof(Data));

        _data->_input_length     = input_length;
        _data->_output_length    = output_length;
        _data->_means            = (BrainSignal)calloc(input_length, sizeof(BrainReal));
        _data->_sigmas           = (BrainSignal)calloc(input_length, sizeof(BrainReal));

        parse_csv_repository(_data, repository_path, tokenizer, is_data_splitted);
    }

    return _data;
}

void
delete_data(BrainData data)
{
    if (data != NULL)
    {
        BrainUint k = 0;
        BrainUint m = data->_training._children;
        if (m < data->_evaluating._children)
        {
            m = data->_evaluating._children;
        }

        for (k = 0; k < m; ++k)
        {
            if (k < data->_evaluating._children)
            {
                free(data->_evaluating._input[k]);
                free(data->_evaluating._output[k]);
            }
            if (k < data->_training._children)
            {
                free(data->_training._input[k]);
                free(data->_training._output[k]);
            }
        }

        free(data->_evaluating._input);
        free(data->_evaluating._output);

        if (data->_training._input)
        {
            free(data->_training._input);
            free(data->_training._output);
        }

        if (data->_means != NULL)
        {
            free(data->_means);
        }

        if (data->_sigmas != NULL)
        {
            free(data->_sigmas);
        }

        free(data);
    }
}

BrainUint
get_number_of_evaluating_sample(const BrainData data)
{
    BrainUint ret = 0;

    if (data)
    {
        ret = data->_evaluating._children;
    }

    return ret;
}

BrainSignal
get_evaluating_input_signal(const BrainData data, const BrainUint index)
{
    BrainSignal ret = NULL;

    if ((data != NULL)
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

    if ((data != NULL)
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

    if (data)
    {
        ret = data->_training._children;
    }

    return ret;
}

BrainSignal
get_training_input_signal(const BrainData data, const BrainUint index)
{
    BrainSignal ret = NULL;

    if ((data != NULL)
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

    if ((data != NULL)
    &&  (index < data->_training._children))
    {
        ret = data->_training._output[index];
    }

    return ret;
}

BrainUint
get_input_signal_length(const BrainData data)
{
    if (data != NULL)
    {
        return data->_input_length;
    }

    return 0;
}

BrainUint
get_output_signal_length(const BrainData data)
{
    if (data != NULL)
    {
        return data->_output_length;
    }

    return 0;
}

BrainSignal
get_input_means(const BrainData data)
{
    BrainSignal ret = NULL;

    if (data != NULL)
    {
        ret = data->_means;
    }

    return ret;
}

BrainSignal
get_input_sigmas(const BrainData data)
{
    BrainSignal ret = NULL;

    if (data != NULL)
    {
        ret = data->_sigmas;
    }

    return ret;
}
