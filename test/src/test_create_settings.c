#include "config_all.h"
#include "brain_settings.h"
#include "brain_learning.h"
#include "brain_activation.h"
#include "brain_cost.h"

int
main(int argc, char** argv)
{
    BrainInt result = EXIT_FAILURE;

    new_settings_from_context(TEST_CREATE_SETTINGS);

    const BrainUint   iterations           = get_settings_max_iterations();
    const BrainDouble dropout_percent      = get_settings_dropout_percent();
    const BrainDouble learning_rate        = get_settings_backpropagation_learning_rate();
    const BrainDouble error                = get_settings_target_error();
    const BrainBool   is_dropout_activated = get_settings_dropout_activated();
    LearningPtrFunc   learning             = get_settings_learning_function();
    ActivationPtrFunc activation_function  = get_settings_neuron_activation();
    CostPtrFunc       cost                 = get_settings_network_cost_function();

    if ((learning             == get_learning_function(BackPropagation)) &&
        (is_dropout_activated == BRAIN_TRUE)                             &&
        (dropout_percent      == 0.5)                                    &&
        (activation_function  == activation(Sigmoid))                    &&
        (cost                 == get_cost_function(Quadratic))           &&
        (learning_rate        == 0.01)                                   &&
        (iterations           == 3000)                                   &&
        (error                == 0.01))
    {
        result = EXIT_SUCCESS;
    }

    delete_settings();

    return result;
}
