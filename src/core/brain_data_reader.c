#include "brain_data_reader.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "brain_logging_utils.h"

struct Data
{
    BrainSignal*  _signals;
    BrainSignal* _observations;
    BrainInt*    _subset;
    BrainInt     _subset_length;
    BrainInt     _signal_length;
    BrainInt     _observation_length;
    BrainInt     _number_of_signal;
} Data;

BrainInt
get_data_next_random_subset_index(const BrainInt* subset,
                             const BrainInt length,
                             const BrainInt max_index)
{
    BrainInt  random_number = -1;

    if (subset && length > 0 && 0 < max_index)
    {
        BrainBool is_already_in_subset = BRAIN_FALSE;

        do
        {
            BrainInt i = 0;
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

BrainInt
get_data_output_length(const BrainData data)
{
    if (data)
    {
        return data->_observation_length;
    }

    return 0;
}

BrainInt
get_data_input_length(const BrainData data)
{
    if (data)
    {
        return data->_signal_length;
    }

    return 0;
}

static BrainInt
get_data_subset_length(const BrainData data)
{
    if (data)
    {
        return data->_subset_length;
    }

    return 0;
}

static BrainInt
get_data_subset_index(const BrainData data,
                      const BrainInt subset_index)
{
    if (data
    && (0 <= subset_index)
    && (subset_index < data->_subset_length))
    {
        return data->_subset[subset_index];
    }

    return -1;
}

BrainData
new_data_from_context(Context context)
{
    BrainChar* buffer = NULL;
    BrainChar* part = NULL;
    BrainData _data = NULL;
    BrainInt i = 0, j = 0;

    if (context && !is_node_with_name(context, "data"))
    {
        BRAIN_CRITICAL("<%s, %d>: Receive an invalid context\n", __FILE__, __LINE__);
        return NULL;
    }

    srand(time(NULL));

    _data = (BrainData)calloc(1, sizeof(Data));
    _data->_number_of_signal   = get_number_of_node_with_name(context, "signal");
    _data->_signal_length      = node_get_int(context, "signal-length", 0);
    _data->_observation_length = node_get_int(context, "observation-length", 0);

    _data->_signals            = (BrainSignal *)calloc(_data->_number_of_signal, sizeof(BrainSignal));
    _data->_observations       = (BrainSignal *)calloc(_data->_number_of_signal, sizeof(BrainSignal));

    _data->_subset_length      = (BrainInt)(_data->_number_of_signal / 2);
    _data->_subset             = (BrainInt *)calloc(_data->_subset_length, sizeof(BrainInt));

    BRAIN_INFO("Number of signal : %d, signal length : %d", _data->_number_of_signal,
                                                            _data->_signal_length);

    for (i = 0; i < _data->_number_of_signal; ++i)
    {
        Context signal_context      = get_node_with_name_and_index(context, "signal", i);

        if (signal_context)
        {
            _data->_signals[i]      = (BrainSignal)calloc(_data->_signal_length, sizeof(BrainDouble));
            _data->_observations[i] = (BrainSignal)calloc(_data->_observation_length, sizeof(BrainDouble));

            buffer = (BrainChar *)node_get_prop(signal_context, "input");
            part = strtok(buffer, ", ");

            for (j = 0; j < _data->_signal_length; ++j)
            {
                if (part != NULL)
                {
                    sscanf(part, "%lf", &_data->_signals[i][j]);
                    part = strtok(NULL, ", ");
                }

                BRAIN_INFO("signal[%d][%d] : %lf", i, j, _data->_signals[i][j]);
            }

            if (buffer)
                free(buffer);

            buffer = (BrainChar *)node_get_prop(signal_context, "output");
            part = strtok(buffer, ", ");

            for (j = 0; j < _data->_observation_length; ++j)
            {
                if (part != NULL)
                {
                    sscanf(part, "%lf", &_data->_observations[i][j]);
                    part = strtok(NULL, ", ");
                }

                BRAIN_INFO("observation[%d][%d] : %lf", i, j, _data->_observations[i][j]);
            }

            if (buffer)
                free(buffer);
        }
    }

    memset(_data->_subset, -1, (_data->_subset_length) * sizeof(BrainInt));

    for (i = 0; i < _data->_subset_length; ++i)
    {
        _data->_subset[i] = get_data_next_random_subset_index(_data->_subset, _data->_subset_length, _data->_number_of_signal);

        BRAIN_INFO("Subset index %d : %d", i, _data->_subset[i]);
    }

    return _data;
}

void
delete_data(BrainData data)
{
    if (data)
    {
        BrainInt i = 0;

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

        if (data->_subset)
        {
            free(data->_subset);
        }

        BRAIN_DEBUG("data has been deleted");
        free(data);
    }
}

BrainSignal
get_data_output(const BrainData data, const BrainInt index)
{
    if (data != NULL && 0 <= index && index < data->_number_of_signal)
    {
        return data->_observations[index];
    }

    return NULL;
}

BrainSignal
get_data_input(const BrainData data, const BrainInt index)
{
    if (data && 0 <= index && index < data->_number_of_signal)
    {
        return data->_signals[index];
    }

    return NULL;
}


BrainInt
get_data_random_subset_index(const BrainData data)
{
    if (data)
    {
        const BrainInt subset_index = rand() % get_data_subset_length(data);
        return get_data_subset_index(data, subset_index);
    }

    return -1;
}
