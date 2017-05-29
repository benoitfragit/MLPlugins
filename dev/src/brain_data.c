#include "brain_data.h"
#include "brain_random.h"

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
