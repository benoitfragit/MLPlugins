#include "brain_data_reader.h"

/**
 * \struct Data
 * \brief  Internal model for a BrainData
 *
 * All protected fields for a BrainData
 */
struct Data
{
    BrainSignal* _signals;            /*!< An array of input signal       */
    BrainSignal* _observations;       /*!< An array of output signal      */
    BrainUint*   _subset;             /*!< An array of index              */
    BrainUint    _subset_length;      /*!< number of pre-randomized index */
    BrainUint    _signal_length;      /*!< Size of the input signal       */
    BrainUint    _observation_length; /*!< Size of the output signal      */
    BrainUint    _number_of_signal;   /*!< Number of training data        */
    BrainUint*   _number_of_chunks;   /*!< Number of chunk per signal     */
} Data;

BrainInt
get_data_next_random_subset_index(const BrainUint* subset,
                                  const BrainUint length,
                                  const BrainUint max_index)
{
    BrainUint random_number = 0;

    if ((subset != NULL)
     && (0 != length)
     && (0 != max_index))
    {
        BrainBool is_already_in_subset = BRAIN_FALSE;

        do
        {
            BrainUint i = 0;
            random_number = (int)(rand() % max_index);

            is_already_in_subset = BRAIN_FALSE;

            for (i = 0; i < length; ++i)
            {
                if (subset[i] == random_number)
                {
                    is_already_in_subset = BRAIN_TRUE;
                    break;
                }
            }
        } while (is_already_in_subset);
    }

    return random_number;
}

BrainUint
get_data_output_length(const BrainData data)
{
    if (data != NULL)
    {
        return data->_observation_length;
    }

    return 0;
}

BrainUint
get_data_input_length(const BrainData data)
{
    if (data != NULL)
    {
        return data->_signal_length;
    }

    return 0;
}

static BrainUint
get_data_subset_length(const BrainData data)
{
    if (data != NULL)
    {
        return data->_subset_length;
    }

    return 0;
}

static BrainInt
get_data_subset_index(const BrainData data,
                      const BrainUint subset_index)
{
    if ((data != NULL)
    && (subset_index < data->_subset_length))
    {
        return data->_subset[subset_index];
    }

    return -1;
}

static void
initialize_data(BrainData data)
{
    if (data != NULL)
    {
        BrainUint i = 0;

        for (i = 0; i < data->_subset_length; ++i)
        {
            data->_subset[i] = get_data_next_random_subset_index(data->_subset,
                                                                 data->_subset_length,
                                                                 data->_number_of_signal);
        }
    }
}

BrainData
new_data(const BrainUint number_of_signals,
         const BrainUint signal_length,
         const BrainUint observation_length)
{

    BrainData _data            = (BrainData)calloc(1, sizeof(Data));
    _data->_number_of_signal   = number_of_signals;
    _data->_signal_length      = signal_length;
    _data->_observation_length = observation_length;
    _data->_signals            = (BrainSignal *)calloc(_data->_number_of_signal, sizeof(BrainSignal));
    _data->_observations       = (BrainSignal *)calloc(_data->_number_of_signal, sizeof(BrainSignal));
    _data->_number_of_chunks   = (BrainUint *)calloc(_data->_number_of_signal, sizeof(BrainUint));
    _data->_subset_length      = (BrainUint)(_data->_number_of_signal / 2);
    _data->_subset             = (BrainUint *)calloc(_data->_subset_length, sizeof(BrainUint));

    initialize_data(_data);

    return _data;
}

void
append_data(BrainData data,
            const BrainUint index,
            BrainSignal input,
            BrainSignal output)
{
    if ((data != NULL)
    &&  (index < data->_number_of_signal))
    {
        ++(data->_number_of_chunks[index]);
        data->_signals[index]      = input;
        data->_observations[index] = output;
    }
}

void
delete_data(BrainData data)
{
    if (data)
    {
        BrainUint i = 0;

        if (data->_signals)
        {
            for (i = 0; i < data->_number_of_signal; ++i)
            {
                if (data->_signals[i])
                {
                    free(data->_signals[i]);
                }
            }

            free(data->_signals);
        }

        if (data->_observations)
        {
            for (i = 0; i < data->_number_of_signal; ++i)
            {
                if (data->_observations[i])
                {
                    free(data->_observations[i]);
                }
            }

            free(data->_observations);
        }

        if (data->_number_of_chunks != NULL)
        {
            free(data->_number_of_chunks);
        }

        if (data->_subset)
        {
            free(data->_subset);
        }

        free(data);
    }
}

BrainSignal
get_data_output(const BrainData data,
                const BrainUint index)
{
    if ((data != NULL)
    &&  (index < data->_number_of_signal))
    {
        return data->_observations[index];
    }

    return NULL;
}

BrainSignal
get_data_input(const BrainData data,
               const BrainUint index,
               const BrainUint chunk_index)
{
    if ((data != NULL)
    &&  (index < data->_number_of_signal)
    &&  (chunk_index < data->_number_of_chunks[index]))
    {
        return &(data->_signals[index][chunk_index * data->_signal_length]);
    }

    return NULL;
}

BrainUint
get_data_number_of_chunks(const BrainData data,
                          const BrainUint signal_index)
{
    if ((data != NULL)
    &&  (signal_index < data->_number_of_signal))
    {
        return data->_number_of_chunks[signal_index];
    }

    return 0;
}

BrainUint
get_data_random_subset_index(const BrainData data)
{
    if (data != NULL)
    {
        const BrainUint subset_index = rand() % get_data_subset_length(data);
        return get_data_subset_index(data, subset_index);
    }

    return 0;
}
