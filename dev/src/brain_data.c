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
struct Node
{
    BrainSignal _input;    /*!< The input signal */
    BrainSignal _output;   /*!< The output signal */
    BrainNode   _next;     /*!< The next node */
} Node;

/**
 * \struct Data
 * \brief  Internal model for a BrainData
 *
 * All protected fields for a BrainData
 */
struct Data
{
    BrainNode   _training;         /*!< This is the training node       */
    BrainNode   _evaluating;       /*!< This is the evaluating node     */
    BrainBool   _is_data_splitted; /*!< is data automatically splitter  */
    BrainUint   _input_length;     /*!< input signal length             */
    BrainUint   _output_length;    /*!< output signal length            */
    BrainSignal _means;            /*!< mean input vector               */
    BrainSignal _sigmas;           /*!< standerd deviation input vector */
    BrainUint   _children;         /*!< number of children */
} Data;

typedef BrainChar Array[50];
typedef Array* BrainLabels;

static BrainChar*
parse_line(BrainChar* line,
           const BrainUint size,
           const BrainString tokenizer,
           BrainSignal signal)
{
    BrainChar* buffer = NULL;

    if (line      != NULL &&
        tokenizer != NULL &&
        signal    != NULL )
    {
        // reading output signal
        buffer = strtok(line, tokenizer);
        BrainUint k = 0;

        for (k = 0; k < size; ++k)
        {
            if (buffer != NULL)
            {
                sscanf(buffer, "%lf", &(signal[k]));
                buffer = strtok(NULL, tokenizer);
            }
        }
    }

    return buffer;
}

static void
parse_labels(BrainChar* line,
             const BrainUint length,
             BrainSignal output,
             BrainLabels labels)
{
    if (line != NULL && output != NULL)
    {
        BrainUint index = 0;
        BrainInt i = -1;
        BrainUint line_size = 0;
        BrainChar* start = line;
        const BrainUint size = sizeof(Array);

        Array buffer;

        do {
            ++line_size;
            ++start;
        } while (start != NULL && *start != '\n' && *start != '\0');

        // we only keep three first caractere
        if (size < line_size)
        {
            line_size = size;
        }

        memcpy(buffer, line, line_size * sizeof(BrainChar));

        for (index = 0; index < length; ++index)
        {
            if (!strcmp(buffer, labels[index]))
            {
                output[index] = 1.0;
                return;
            }

            if (labels[index][0] == '\0' && i == -1)
            {
                i = index;
            }
        }

        if (0 <= i)
        {
            memcpy(labels[i], buffer, sizeof(Array));
            output[i] = 1.0;
        }
    }
}

static void
parse_csv_repository(BrainData       data,
                     BrainString     repository_path,
                     BrainString     tokenizer,
                     const BrainBool line_break,
                     const BrainBool classify)
{
    if ((data            != NULL) &&
        (repository_path != NULL) &&
        (tokenizer       != NULL) )
    {
        // opening the file for reading
        FILE *repository = fopen(repository_path, "r");

        if (repository != NULL)
        {
            const BrainUint input_length = get_input_signal_length(data);
            const BrainUint output_length = get_output_signal_length(data);

            BrainBool  read_input = BRAIN_TRUE;
            BrainChar* line   = NULL;
            size_t     len    = 0;

            BrainSignal input = NULL;
            BrainSignal output = NULL;

            BrainLabels labels = NULL;
            if (classify)
            {
                labels = (BrainLabels)calloc(output_length, sizeof(Array));
                memset(labels, 0, output_length*sizeof(Array));
            }

            while (getline(&line, &len, repository) != -1)
            {
                if ((len > 0) && (line != NULL))
                {
                    BrainChar* buffer = NULL;

                    // remove ending '\n'
                    line[len - 1] = '\0';

                    // parse input signal
                    if (read_input)
                    {
                        input  = (BrainSignal)calloc(input_length,  sizeof(BrainDouble));

                        buffer = parse_line(line,
                                            input_length,
                                            tokenizer,
                                            input);
                    }

                    // parse output signal if needed
                    if (!line_break || !read_input)
                    {
                        BrainChar* output_line = line;

                        if (!line_break)
                        {
                            output_line = buffer;
                        }

                        output = (BrainSignal)calloc(output_length, sizeof(BrainDouble));

                        if (!classify)
                        {
                            parse_line(output_line,
                                       output_length,
                                       tokenizer,
                                       output);
                        }
                        else
                        {
                            parse_labels(output_line,
                                         output_length,
                                         output,
                                         labels);
                        }

                        //output should be read next time
                        read_input = BRAIN_TRUE;

                        // add this couple of input and output
                        new_node(data, input, output);

                        input  = NULL;
                        output = NULL;
                    }
                    else
                    {
                        if (read_input)
                        {
                            read_input = BRAIN_FALSE;
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
}

BrainData
new_data(const BrainUint input_length,
         const BrainUint output_length,
         const BrainBool is_data_splitted)
{
    BrainData _data = (BrainData)calloc(1, sizeof(Data));

    _data->_training         = NULL;
    _data->_evaluating       = NULL;
    _data->_is_data_splitted = is_data_splitted;
    _data->_input_length     = input_length;
    _data->_output_length    = output_length;
    _data->_means            = (BrainSignal)calloc(input_length, sizeof(BrainDouble));
    _data->_sigmas           = (BrainSignal)calloc(input_length, sizeof(BrainDouble));
    _data->_children         = 0;

    return _data;
}

static void
compute_data_means(const BrainNode node,
                   const BrainUint input_length,
                   const BrainUint number_of_signal,
                   BrainSignal means)
{
    if (node != NULL && means != NULL)
    {
        if (node->_input != NULL)
        {
            BrainUint index = 0;

            for (index = 0; index < input_length; ++index)
            {
                means[index] += node->_input[index] / (BrainDouble)number_of_signal;
            }
        }

        compute_data_means(node->_next,  input_length, number_of_signal, means);
    }
}

static void
compute_data_sigmas(const BrainNode node,
                    const BrainUint input_length,
                    const BrainUint number_of_signal,
                    BrainSignal sigmas)
{
    if (node != NULL && sigmas != NULL)
    {
        if (node->_input != NULL)
        {
            BrainUint index = 0;

            for (index = 0; index < input_length; ++index)
            {
                sigmas[index] += node->_input[index]*node->_input[index] / (BrainDouble)number_of_signal;
            }
        }

        compute_data_sigmas(node->_next,  input_length, number_of_signal, sigmas);
    }
}

static void
center_data(BrainNode node, const BrainUint input_length, const BrainSignal means)
{
    if (node != NULL && means != NULL)
    {
        if (node->_input != NULL)
        {
            BrainUint index = 0;

            for (index = 0; index < input_length; ++index)
            {
                node->_input[index] -= means[index];
            }
        }

        center_data(node->_next, input_length, means);
    }
}

static void
normalize_data(BrainNode node, const BrainUint input_length, const BrainSignal sigmas)
{
    if (node != NULL && sigmas != NULL)
    {
        if (node->_input != NULL)
        {
            BrainUint index = 0;

            for (index = 0; index < input_length; ++index)
            {
                node->_input[index] /= sigmas[index];
            }
        }

        normalize_data(node->_next,  input_length, sigmas);
    }
}

void
preprocess(BrainData data)
{
    if (data != NULL && data->_training != NULL)
    {
        const BrainUint number_of_signal = data->_children;

        // data centering
        compute_data_means(data->_training,
                           data->_input_length,
                           number_of_signal,
                           data->_means);

        center_data(data->_training,
                    data->_input_length,
                    data->_means);

        // data normalization
        compute_data_sigmas(data->_training,
                            data->_input_length,
                            number_of_signal,
                            data->_sigmas);

        normalize_data(data->_training,
                       data->_input_length,
                       data->_sigmas);
    }
}

static void
delete_node(BrainNode node)
{
    if (node != NULL)
    {
        delete_node(node->_next);

        if (node->_input != NULL)
        {
            free(node->_input);
        }

        if (node->_output != NULL)
        {
            free(node->_output);
        }

        free(node);
    }
}

void
delete_data(BrainData data)
{
    if (data != NULL)
    {
        delete_node(data->_training);
        delete_node(data->_evaluating);

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

BrainNode
get_training_node(const BrainData data)
{
    if (data != NULL)
    {
        return data->_training;
    }

    return NULL;
}

BrainNode
get_evaluating_node(const BrainData data)
{
    if (data != NULL)
    {
        return data->_evaluating;
    }

    return NULL;
}

BrainNode
get_next_node(const BrainNode node)
{
    if (node != NULL)
    {
        return node->_next;
    }

    return NULL;
}

BrainSignal
get_node_input_signal(const BrainNode node)
{
    if (node != NULL)
    {
        return node->_input;
    }

    return NULL;
}

BrainSignal
get_node_output_signal(const BrainNode node)
{
    if (node != NULL)
    {
        return node->_output;
    }

    return NULL;
}

void
new_node(BrainData data, BrainSignal input, BrainSignal output)
{
    if (data != NULL)
    {
        BrainNode new_node = (BrainNode)calloc(1, sizeof(Node));

        new_node->_input  = input;
        new_node->_output = output;

        data->_children   += 1;

        // data auto separation into 2 sets testing
        if ((data->_is_data_splitted == BRAIN_TRUE) && get_random_double_value() < 0.5)
        {
            new_node->_next = data->_training;
            data->_training = new_node;
        }
        else
        {
            new_node->_next = data->_evaluating;
            data->_evaluating = new_node;
        }
    }
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

BrainUint
get_node_children(const BrainData data)
{
    if (data != NULL)
    {
        return data->_children;
    }

    return 0;
}
