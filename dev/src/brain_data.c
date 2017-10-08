#include "brain_data.h"
#include "brain_logging_utils.h"
#include "brain_random.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * \struct Node
 * \brief  Internal model for a BrainNode
 *
 * All protected fields for a BrainNode
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

    BrainUint   _children;         /*!< number of children */
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
            BrainUint  k = 0;
            BrainUint  j = 0;
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
            data->_training._input    = (BrainSignal*)calloc(counter, sizeof(BrainSignal));
            data->_training._output   = (BrainSignal*)calloc(counter, sizeof(BrainSignal));
            data->_evaluating._input  = (BrainSignal*)calloc(counter, sizeof(BrainSignal));
            data->_evaluating._output = (BrainSignal*)calloc(counter, sizeof(BrainSignal));
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

                    // remove ending '\n'
                    line[len - 1] = '\0';

                    /****************************************************************/
                    /**              Randomly choose signal storage                **/
                    /****************************************************************/
                    Dataset* dataset = &(data->_training);
                    if ((is_data_splitted == BRAIN_FALSE) ||
                        (0.55 < get_random_double_value()))
                    {
                        dataset = &(data->_evaluating);
                    }

                    dataset->_input[counter]  = (BrainSignal)calloc(input_length, sizeof(BrainDouble));
                    dataset->_output[counter] = (BrainSignal)calloc(output_length, sizeof(BrainDouble));

                    /****************************************************************/
                    /**                        Parsing signals                     **/
                    /****************************************************************/
                    buffer = strtok(line, tokenizer);

                    while(k < input_length + output_length)
                    {
                        BrainDouble* value = NULL;
                        BrainUint kp = k;

                        if (k < input_length)
                        {
                            value = &((dataset->_input[counter])[k]);
                        }
                        else
                        {
                            kp -= input_length;

                            if (kp < output_length)
                            {
                                value = &((dataset->_output[counter])[kp]);
                            }
                        }

                        if (buffer != NULL)
                        {
                            sscanf(buffer, "%lf", value);

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
                data->_means[k] /= counter;
                data->_sigmas[k] = (data->_sigmas[k] / counter) - data->_means[k] * data->_means[k];

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
        _data->_means            = (BrainSignal)calloc(input_length, sizeof(BrainDouble));
        _data->_sigmas           = (BrainSignal)calloc(input_length, sizeof(BrainDouble));

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
        free(data->_training._input);
        free(data->_training._output);

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
