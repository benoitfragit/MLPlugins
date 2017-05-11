#include "brain_learning.h"
#include "brain_neuron.h"
#include "brain_weight.h"
#include "brain_settings.h"

#define MIN(a, b) ((a) < (b) ? a : b)
#define MAX(a, b) ((a) < (b) ? b : a)

static BrainString _learning_names[] =
{
    "BackPropagation",
    "Resilient"
};

static void
update_neuron_using_backpropagation(BrainNeuron neuron,
                                    const BrainDouble loss)
{
    if (neuron != NULL)
    {
        const BrainUint number_of_inputs = get_neuron_number_of_input(neuron);
        BrainUint i = 0;

        const BrainDouble learning_rate = get_settings_backpropagation_learning_rate();
        ActivationPtrFunc derivative_function = get_settings_neuron_derivative();

        if (derivative_function != NULL)
        {
            const BrainDouble neuron_derivative = derivative_function(get_neuron_output(neuron));
            const BrainDouble neuron_gradient   = neuron_derivative * loss;

            BrainWeight neuron_bias = get_neuron_bias(neuron);

            set_weight_correction(neuron_bias, - learning_rate * neuron_gradient);
            apply_weight_correction(neuron_bias);

            for (i = 0; i < number_of_inputs; ++i)
            {
                const BrainDouble neuron_gradient_w = neuron_gradient * get_neuron_input(neuron, i);
                BrainWeight neuron_weight = get_neuron_weight(neuron,i);

                update_weight_loss(neuron_weight, loss);

                set_weight_correction(neuron_weight, - learning_rate * neuron_gradient_w);
                apply_weight_correction(neuron_weight);
            }
        }
    }
}

static void
apply_neuron_rprop(const BrainDouble rprop_eta_positive,
                   const BrainDouble rprop_eta_negative,
                   const BrainDouble rprop_delta_max,
                   const BrainDouble rprop_delta_min,
                   const BrainDouble new_weight_gradient,
                   BrainWeight weight)
{
    if (weight != NULL)
    {
        const BrainDouble gradient   = get_weight_gradient(weight);
        const BrainDouble delta      = get_weight_delta(weight);
        const BrainDouble correction = get_weight_correction(weight);

        BrainDouble new_delta = 0.0;
        BrainDouble new_correction = 0.0;

        if (0.0 < gradient * new_weight_gradient)
        {
            new_delta      = MIN(delta * rprop_eta_positive, rprop_delta_max);
            new_correction = new_delta;

            if (0.0 < new_weight_gradient)
            {
                new_correction *= -1.0;
            }

            set_weight_delta(weight, new_delta);
            set_weight_gradient(weight, new_weight_gradient);
            set_weight_correction(weight, new_correction);

            apply_weight_correction(weight);
        }
        else if (gradient * new_weight_gradient < 0.0)
        {
            new_delta = MAX(delta * rprop_eta_negative, rprop_delta_min);

            new_correction = -correction;

            set_weight_gradient(weight, 0.0);
            set_weight_correction(weight, new_correction);
            set_weight_delta(weight, delta);

            apply_weight_correction(weight);
        }
        else if (gradient * new_weight_gradient == 0.0)
        {
            new_correction = delta;

            if (0.0 < new_weight_gradient)
            {
                new_correction *= -1.0;
            }

            set_weight_gradient(weight, new_weight_gradient);
            set_weight_correction(weight, new_correction);
            apply_weight_correction(weight);
        }
    }
}

static void
update_neuron_using_resilient(BrainNeuron neuron,
                              const BrainDouble loss)
{
    if (neuron != NULL)
    {
        const BrainDouble rprop_eta_positive  = get_settings_resilient_eta_positive();
        const BrainDouble rprop_eta_negative  = get_settings_resilient_eta_negative();
        const BrainDouble rprop_delta_min     = get_settings_resilient_delta_min();
        const BrainDouble rprop_delta_max     = get_settings_resilient_delta_max();
        ActivationPtrFunc derivative_function = get_settings_neuron_derivative();

        if (derivative_function != NULL)
        {
            const BrainDouble neuron_derivative = derivative_function(get_neuron_output(neuron));
            const BrainDouble neuron_gradient   = neuron_derivative * loss;
            const BrainUint   number_of_inputs  = get_neuron_number_of_input(neuron);
            BrainUint i = 0;

            //first update the bias using RProp algorithm
            apply_neuron_rprop(rprop_eta_positive,
                               rprop_eta_negative,
                               rprop_delta_max,
                               rprop_delta_min,
                               neuron_gradient,
                               get_neuron_bias(neuron));

            //then update all other weights using same technique
            for (i = 0; i < number_of_inputs; ++i)
            {
                const BrainDouble neuron_gradient_w = neuron_gradient * get_neuron_input(neuron, i);

                BrainWeight neuron_weight = get_neuron_weight(neuron, i);
                //this is also valid in RProp mode
                update_weight_loss(neuron_weight, loss);

                apply_neuron_rprop(rprop_eta_positive,
                                   rprop_eta_negative,
                                   rprop_delta_max,
                                   rprop_delta_min,
                                   neuron_gradient_w,
                                   neuron_weight);
            }
        }
    }
}


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
