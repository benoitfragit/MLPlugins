#include "brain_learning.h"
#include "brain_neuron_builder.h"

static BrainString _learning_names[] =
{
    "BackPropagation",
    "Resilient"
};

BrainLearningType
get_learning_type(BrainString learning_name)
{
    BrainUint i = 0;

    for (i = First_Learning; i <= Last_Learning; ++i)
    {
        if (i != First_Learning
        && !strcmp(_learning_names[i - First_Learning - 1], learning_name))
        {
            return i;
        }
    }

    return Invalid_Learning;
}

LearningPtrFunc
get_learning_function(const BrainLearningType learning_type)
{
    switch (learning_type)
    {
        case BackPropagation: return &update_neuron_using_backpropagation;
        case Resilient: return &update_neuron_using_resilient;
        default:
            break;
    }

    return &update_neuron_using_backpropagation;
}
