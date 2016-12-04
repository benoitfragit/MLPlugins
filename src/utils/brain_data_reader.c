#include "brain_data_reader.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "brain_logging_utils.h"

struct Data
{
    Signal*      _signals;
    Observation* _observations;
    int*         _subset;
    int          _subset_length;
    int          _signal_length;
    int          _observation_length;
    int          _number_of_signal;
} Data;

int
get_observation_length(const Data_t data)
{
    if (data)
    {
        return data->_observation_length;
    }

    return 0;
}

int
get_signal_length(const Data_t data)
{
    if (data)
    {
        return data->_signal_length;
    }

    return 0;
}

int
get_subset_length(const Data_t data)
{
    if (data)
    {
        return data->_subset_length;
    }

    return 0;
}

int
get_subset_index(const Data_t data, const int subset_index)
{
    if (data
    && (0 <= subset_index)
    && (subset_index < data->_subset_length))
    {
        return data->_subset[subset_index];
    }

    return -1;
}

Data_t
new_data_from_context(Context context)
{
    char* buffer = NULL;
    char* part = NULL;
    Data_t _data = NULL;
    int i = 0, j = 0;

    if (context && !is_node_with_name(context, "data"))
    {
        BRAIN_CRITICAL("<%s, %d>: Receive an invalid context\n", __FILE__, __LINE__);
        return NULL;
    }

    _data = (Data_t)malloc(sizeof(Data));
    _data->_number_of_signal   = get_number_of_node_with_name(context, "signal");
    _data->_signal_length      = node_get_int(context, "signal-length", 0);
    _data->_observation_length = node_get_int(context, "observation-length", 0);

    _data->_signals            = (Signal *)malloc(_data->_number_of_signal * sizeof(Signal));
    _data->_observations       = (Observation *)malloc(_data->_number_of_signal * sizeof(Observation));

    _data->_subset_length      = (int)(_data->_number_of_signal / 2);
    _data->_subset             = (int *)malloc(_data->_subset_length * sizeof(int));

    BRAIN_INFO("Number of signal : %d, signal length : %d", _data->_number_of_signal,
                                                                           _data->_signal_length);

    for (i = 0; i < _data->_number_of_signal; ++i)
    {
        Context signal_context      = get_node_with_name_and_index(context, "signal", i);

        if (signal_context)
        {
            _data->_signals[i]      = (Signal)malloc(_data->_signal_length * sizeof(double));
            _data->_observations[i] = (Observation)malloc(_data->_observation_length * sizeof(double));

            buffer = (char *)node_get_prop(signal_context, "input");
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

            buffer = (char *)node_get_prop(signal_context, "output");
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

    memset(_data->_subset, -1, (_data->_subset_length) * sizeof(int));

    for (i = 0; i < _data->_subset_length; ++i)
    {
        _data->_subset[i] = get_next_random_subset_index(_data->_subset, _data->_subset_length, _data->_number_of_signal);

        BRAIN_INFO("Subset index %d : %d", i, _data->_subset[i]);
    }

    return _data;
}

void
delete_data(Data_t data)
{
    int i = 0;

    if (data)
    {
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

int
get_next_random_subset_index(const int* subset,
                             const int length,
                             const int max_index)
{
    int is_already_in_subset = 1;
    int i = 0;
    int random_number = -1;

    if (subset && length > 0 && 0 <= max_index)
    {
        do {
            srand(time(NULL));
            random_number = (int)(rand() % max_index);

            is_already_in_subset = 0;

            for (i = 0; i < length; ++i)
            {
                if (subset[i] == random_number)
                {
                    is_already_in_subset = 1;
                    break;
                }
            }
        } while (is_already_in_subset);
    }

    return random_number;
}


Observation
get_observation(Data_t data, const int index)
{
    if (data != NULL && 0 <= index && index < data->_number_of_signal)
    {
        return data->_observations[index];
    }

    return NULL;
}

Signal
get_signal(Data_t data, const int index)
{
    if (data && 0 <= index && index < data->_number_of_signal)
    {
        return data->_signals[index];
    }

    return NULL;
}
