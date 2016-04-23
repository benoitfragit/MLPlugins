#include "brain_data_reader.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

Data*
new_data_from_context(Context context)
{
    Data* _data = NULL;
    int i = 0, j = 0;
    char buff[100];

    if (context && !is_node_with_name(context, "data"))
    {
        fprintf(stderr, "<%s, %d>: Receive an invalid context\n", __FILE__, __LINE__);
        return NULL;
    }

    _data = (Data *)malloc(sizeof(Data));
    _data->_number_of_signal = get_number_of_node_with_name(context, "signal");
    _data->_signal_length    = node_get_int(context, "signal-length", 0);
    _data->_signals          = (Signal *)malloc(_data->_number_of_signal * sizeof(Signal));
    _data->_subset_length    = (int)(_data->_number_of_signal / 2);
    _data->_subset           = (int *)malloc(_data->_subset_length * sizeof(int));

    for (i = 0; i < _data->_number_of_signal; ++i)
    {
        Context signal_context = get_node_with_name_and_index(context, "signal", i);

        if (signal_context)
        {
            _data->_signals[i] = (Signal)malloc(_data->_signal_length * sizeof(double));

            for (j = 0; j < _data->_signal_length; ++j)
            {
                sprintf(buff, "v%d", j);
                _data->_signals[i][j] = node_get_double(signal_context, buff, 0.0);
            }
        }
    }

    // generate subset random index to break data in 2 parts
    // one for the training process and the other one for the evaluation process
    memset(_data->_subset, -1, (_data->_subset_length) * sizeof(int));

    for (i = 0; i < _data->_subset_length; ++i)
    {
        _data->_subset[i] = get_next_random_subset_index(_data->_subset, _data->_subset_length, _data->_signal_length);
    }

    return _data;
}

void
delete_data(Data* data)
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

        if (data->_subset)
        {
            free(data->_subset);
        }

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
            //printf("%d ", random_number);
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


const Signal
signal(Data* data, const int index)
{
    if (data && 0 <= index && index < data->_number_of_signal)
    {
        return data->_signals[index];
    }

    return NULL;
}

