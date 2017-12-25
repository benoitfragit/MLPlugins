#include "config_all.h"
#include "brain_types.h"
#include "brain_data.h"
#include "brain_logging_utils.h"
#include <math.h>

static BrainReal TEST_INPUT_SIGNALS[4][3] = {{1.0, 3.0, 2.0},
                                             {2.0, 9.0, 3.0},
                                             {3.0, 2.0, 7.0},
                                             {55.0, 2.0, 11.0}};
static BrainReal TEST_OUTPUT_SIGNALS[4][1] = {{11.0},
                                              {21.0},
                                              {52.0},
                                              {33.0}};

static const BrainReal ERROR_TEST_THRESHOLD = 1e-4;

static const BrainUint NUMBER_OF_TEST_SIGNAL = 4;

static BrainBool
areEqualSignal(const BrainSignal left,
                  const BrainSignal right,
                  const BrainSignal means,
                  const BrainSignal sigmas,
                  const BrainUint   size)
{
    BrainBool ret = BRAIN_FALSE;

    if ((left   != NULL)
    &&  (right  != NULL)
    &&  (0      != size))
    {
        BrainReal error = 0.0;
        BrainUint i = 0;

        for (i = 0; i < size; ++i)
        {
            BrainReal var = left[i];

            if ((means  != NULL)
            &&  (sigmas != NULL))
            {
                var *= sqrt(sigmas[i]);
                var += means[i];
            }

            error += (var - right[i]) * (var - right[i]);
        }

        if (error < ERROR_TEST_THRESHOLD)
        {
            ret = BRAIN_TRUE;
        }
    }

    return ret;
}

BrainInt
main(int argc, char** argv)
{
    BrainInt result = EXIT_SUCCESS;

    BrainData data = new_data(TEST_DATA_LOADING_PATH, ",", 3, 1, BRAIN_TRUE, BRAIN_FALSE, BRAIN_TRUE);

    if (data)
    {
        const BrainUint input_length  = get_input_signal_length(data);
        const BrainUint output_length = get_output_signal_length(data);

        if ((input_length == 3)
        &&  (output_length == 1))
        {
            const BrainUint number_of_training_sample = get_number_of_training_sample(data);
            const BrainUint number_of_evaluating_sample = get_number_of_evaluating_sample(data);

            if (number_of_training_sample + number_of_evaluating_sample == NUMBER_OF_TEST_SIGNAL)
            {
                const BrainSignal training_means    = get_training_means(data);
                const BrainSignal training_sigmas   = get_training_sigmas(data);
                const BrainSignal evaluating_means  = get_evaluating_means(data);
                const BrainSignal evaluating_sigmas = get_evaluating_sigmas(data);

                BrainUint i = 0;
                BrainUint j = 0;
                BrainUint n = number_of_training_sample;

                if (n < number_of_evaluating_sample)
                {
                    n = number_of_evaluating_sample;
                }

                for (i = 0; i < 4; ++i)
                {
                    BrainBool found = BRAIN_FALSE;

                    for (j = 0; j < n; ++j)
                    {
                        if (j < number_of_training_sample)
                        {
                            const BrainSignal input_signal = get_training_input_signal(data, j);
                            const BrainSignal output_signal = get_training_output_signal(data, j);

                            if (areEqualSignal(output_signal,
                                                TEST_OUTPUT_SIGNALS[i],
                                                NULL,
                                                NULL,
                                                output_length)
                            &&  areEqualSignal(input_signal,
                                                TEST_INPUT_SIGNALS[i],
                                                training_means,
                                                training_sigmas,
                                                input_length))
                            {
                                found = BRAIN_TRUE;

                                break;
                            }
                        }

                        if (j < number_of_evaluating_sample)
                        {
                            const BrainSignal input_signal  = get_evaluating_input_signal(data, j);
                            const BrainSignal output_signal = get_evaluating_output_signal(data, j);

                            if (areEqualSignal(output_signal,
                                                TEST_OUTPUT_SIGNALS[i],
                                                NULL,
                                                NULL,
                                                output_length)
                            &&  areEqualSignal(input_signal,
                                                TEST_INPUT_SIGNALS[i],
                                                evaluating_means,
                                                evaluating_sigmas,
                                                input_length))
                            {
                                found = BRAIN_TRUE;

                                break;
                            }
                        }
                    }

                    if (!found)
                    {
                        result = EXIT_FAILURE;

                        break;
                    }
                }
            }
            else
            {
                BRAIN_CRITICAL("Number of total signal are not equal\n");

                result = EXIT_FAILURE;
            }
        }
        else
        {
            BRAIN_CRITICAL("Signal size are not equals\n");

            result = EXIT_FAILURE;
        }

        delete_data(data);
    }
    else
    {
        BRAIN_CRITICAL("Impossible to create a BrainData");

        result= EXIT_FAILURE;
    }

    return result;
}
