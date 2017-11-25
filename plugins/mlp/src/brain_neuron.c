#include "brain_neuron.h"
#include "brain_activation.h"
#include "brain_random.h"
#include "brain_xml_utils.h"
#include "brain_logging_utils.h"

#include <math.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

#define EPSILON 1e-6

/**
 * \struct Neuron
 * \brief  Internal model for a BrainNeuron
 *
 * All protected fields for a BrainNeuron
 */
struct Neuron
{
    /******************************************************************/
    /**                      FUNCTIONAL PARAMETERS                   **/
    /******************************************************************/
    ActivationPtrFunc _activation_function;   /*!< Activation function                                  */
    ActivationPtrFunc _derivative_function;   /*!< Derivative function                                  */
    LearningPtrFunc   _learning_function;     /*!< Learning function                                    */
    /******************************************************************/
    /**                      STRUCTURAL PARAMETERS                   **/
    /******************************************************************/
    BrainSignal       _in;                    /*!< Input signal of an BrainNeuron                       */
    BrainSignal       _w;                     /*!< An array of weight without the bias                  */
    BrainSignal       _gradients;             /*!< weights gradient values                              */
    BrainSignal       _deltas;                /*!< weights delta values                                 */
    BrainSignal       _errors;                /*!< error to correct in the layer                        */
    BrainSignal       _out;                   /*!< An output value pointer owned by the BrainLayer      */
    BrainDouble       _bias;                  /*!< Bias of the neuron                                   */
    BrainDouble       _bias_delta;            /*!< Bias delta                                           */
    BrainDouble       _bias_gradient;         /*!< Bias gradient                                        */
    BrainDouble       _sum;                   /*!< Summation of all input time weight                   */
    /******************************************************************/
    /**                        TRAINING PARAMETERS                   **/
    /******************************************************************/
    BrainDouble       _rprop_eta_plus;        /*!< Rprop eta for positive gradient transition           */
    BrainDouble       _rprop_eta_minus;       /*!< Rprop eta for negative gradient transition           */
    BrainDouble       _rprop_delta_min;       /*!< Rprop delta min                                      */
    BrainDouble       _rprop_delta_max;       /*!< Rprop delta max                                      */
    BrainDouble       _backprop_learning_rate;/*!< BackProp learning rate                               */
    BrainDouble       _dropout_ratio;         /*!< Dropout filtering raion                              */
    BrainUint         _number_of_input;       /*!< Number of inputs of the neuron                       */
    BrainBool         _use_dropout;           /*!< Dropout activation                                   */
} Neuron;

static BrainString _learning_names[] =
{
    "BackPropagation",
    "Resilient"
};

static BrainLearningType
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

static void
update_neuron_using_backpropagation(BrainNeuron neuron, const BrainDouble loss)
{
    BRAIN_INPUT(update_neuron_using_backpropagation)

    if (neuron != NULL)
    {
        const BrainUint   number_of_inputs      = neuron->_number_of_input;
        const BrainDouble learning_rate         = neuron->_backprop_learning_rate;
        ActivationPtrFunc derivative_function   = neuron->_derivative_function;

        if (derivative_function != NULL)
        {
            const BrainDouble neuron_gradient   = loss * derivative_function(neuron->_sum);
            BrainUint i = 0;
            /******************************************************/
            /**               BACKPROPAGATE $_i                  **/
            /******************************************************/
            neuron->_bias -= learning_rate * neuron_gradient;

            for (i = 0; i < number_of_inputs; ++i)
            {
                neuron->_gradients[i]    =  neuron_gradient;
                neuron->_deltas[i]       =  neuron_gradient * neuron->_in[i];

                if (neuron->_errors)
                {
                    neuron->_errors[i]   += neuron_gradient * neuron->_w[i];
                }

                neuron->_w[i]           -= learning_rate * neuron->_deltas[i];
            }
        }
    }

    BRAIN_OUTPUT(update_neuron_using_backpropagation)
}

static void
update_neuron_using_resilient(BrainNeuron neuron,
                              const BrainDouble loss)
{
    BRAIN_INPUT(update_neuron_using_resilient)

    if (neuron != NULL)
    {
        const BrainDouble rprop_eta_positive  = neuron->_rprop_eta_plus;
        const BrainDouble rprop_eta_negative  = neuron->_rprop_eta_minus;
        const BrainDouble rprop_delta_min     = neuron->_rprop_delta_min;
        const BrainDouble rprop_delta_max     = neuron->_rprop_delta_max;
        ActivationPtrFunc derivative_function = neuron->_derivative_function;

        if (derivative_function != NULL)
        {
            const BrainDouble neuron_gradient   = loss * derivative_function(neuron->_sum);
            const BrainUint   number_of_inputs  = neuron->_number_of_input;
            BrainUint i                         = 0;
            BrainDouble new_delta               = 0.0;
            BrainDouble new_correction          = 0.0;
            BrainDouble new_gradient            = 0.0;
            BrainDouble g                       = neuron_gradient;

            //first update the bias using RProp algorithm
            if (0.0 < neuron->_bias_gradient * g)
            {
                new_delta      = MIN(neuron->_bias_delta * rprop_eta_positive, rprop_delta_max);
                new_correction = new_delta;
                new_gradient   = g;

                if (0.0 < g)
                {
                    new_correction *= -1.0;
                }
            }
            else if (neuron->_bias_gradient * g < 0.0)
            {
                new_delta       = MAX(neuron->_bias_delta * rprop_eta_negative, rprop_delta_min);
            }
            else if (fabs(neuron->_bias_gradient * g) <= EPSILON)
            {
                new_correction  = neuron->_bias_delta;
                new_gradient    = g;

                if (0.0 < g)
                {
                    new_correction *= -1.0;
                }
            }

            neuron->_bias         += new_correction;
            neuron->_bias_delta    = new_delta;
            neuron->_bias_gradient = new_gradient;

            //then update all other weights using same technique
            for (i = 0; i < number_of_inputs; ++i)
            {
                g               = neuron_gradient * neuron->_in[i];
                new_delta       = 0.0;
                new_correction  = 0.0;
                new_gradient    = 0.0;

                // Update error in the previous error
                if (neuron->_errors != NULL)
                {
                    neuron->_errors[i] += neuron_gradient * neuron->_w[i];
                }

                if (0.0 < neuron->_gradients[i] * g)
                {
                    new_delta      = MIN(neuron->_deltas[i] * rprop_eta_positive, rprop_delta_max);
                    new_correction = new_delta;
                    new_gradient   = g;

                    if (0.0 < g)
                    {
                        new_correction *= -1.0;
                    }
                }
                else if (neuron->_gradients[i] * g < 0.0)
                {
                    new_delta       = MAX(neuron->_deltas[i] * rprop_eta_negative, rprop_delta_min);
                }
                else if (fabs(neuron->_gradients[i] * g) <= EPSILON)
                {
                    new_correction  = neuron->_deltas[i];
                    new_gradient    = g;

                    if (0.0 < g)
                    {
                        new_correction *= -1.0;
                    }
                }

                neuron->_w[i]        += new_correction;
                neuron->_deltas[i]    = new_delta;
                neuron->_gradients[i] = new_gradient;
            }
        }
    }

    BRAIN_OUTPUT(update_neuron_using_resilient)
}

static LearningPtrFunc
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

void
configure_neuron_with_context(BrainNeuron neuron, Context context)
{
    BRAIN_INPUT(configure_neuron_with_context)

    BrainLearningType     learning_type      = BackPropagation;
    BrainActivationType   activation_type    = Sigmoid;

    if (neuron && context)
    {
        BrainChar* buffer = NULL;
        Context training_context = get_node_with_name_and_index(context, "training", 0);

        if (training_context != NULL)
        {
            Context dropout_context = get_node_with_name_and_index(training_context, "dropout", 0);
            Context method_context  = get_node_with_name_and_index(training_context, "method", 0);

            if (dropout_context != NULL)
            {
                neuron->_use_dropout   = node_get_bool(dropout_context, "activate", BRAIN_FALSE);
                neuron->_dropout_ratio = node_get_double(dropout_context, "factor", 1.0);
            }

            buffer = (BrainChar *)node_get_prop(training_context, "learning");
            learning_type = get_learning_type(buffer);
            neuron->_learning_function = get_learning_function(learning_type);

            if (buffer != NULL)
            {
                free(buffer);
            }

            if (method_context != NULL)
            {
                switch (learning_type)
                {
                    case BackPropagation:
                    {
                        Context backprop_context = get_node_with_name_and_index(method_context, "backprop", 0);

                        if (backprop_context != NULL)
                        {
                            neuron->_backprop_learning_rate  = node_get_double(backprop_context, "learning-rate", 1.2);
                        }
                    }
                        break;
                    case Resilient:
                    {
                        Context rprop_context = get_node_with_name_and_index(method_context, "rprop", 0);

                        if (rprop_context != NULL)
                        {
                            Context eta_context   = get_node_with_name_and_index(rprop_context, "resilient-eta", 0);
                            Context delta_context = get_node_with_name_and_index(rprop_context, "resilient-delta", 0);

                            if (eta_context != NULL)
                            {
                                neuron->_rprop_eta_plus  = node_get_double(eta_context, "positive", 1.25);
                                neuron->_rprop_eta_minus = node_get_double(eta_context, "negative", 0.95);
                            }

                            if (delta_context != NULL)
                            {
                                neuron->_rprop_delta_max = node_get_double(delta_context, "max", 50.0);
                                neuron->_rprop_delta_min = node_get_double(delta_context, "min", 0.000001);
                            }
                        }
                    }
                        break;
                    default:
                        break;
                }
            }
        }

        buffer = (BrainChar *)node_get_prop(context, "activation-function");
        activation_type = get_activation_type(buffer);
        neuron->_activation_function = activation(activation_type);
        neuron->_derivative_function = derivative(activation_type);

        if (buffer != NULL)
        {
            free(buffer);
        }
    }

    BRAIN_OUTPUT(configure_neuron_with_context)
}

void
activate_neuron(BrainNeuron neuron,
                const BrainBool is_an_hidden_unit)
{
    BRAIN_INPUT(activate_neuron)

    if (neuron != NULL)
    {
        ActivationPtrFunc activation_function = neuron->_activation_function;
        const BrainDouble dropout_percent     = neuron->_dropout_ratio;
        const BrainBool   use_dropout         = neuron->_use_dropout;
        BrainDouble       dropout_factor      = 1.0;

        *(neuron->_out) = 0.0;
        neuron->_sum    = 0.0;

        /**************************************************************/
        /**               APPLY DROPOUT REJECTION                    **/
        /**************************************************************/
        if (use_dropout
        &&  is_an_hidden_unit
        && (0 <= dropout_percent)
        && (dropout_percent < 1.0))
        {
            if (get_random_double_value() < dropout_percent)
            {
                dropout_factor = 0.0;
            }
            else
            {
                dropout_factor *= 1.0 / (1.0 - dropout_percent);
            }
        }

        /**************************************************************/
        /**                 COMPUTE A(<in, W>)                       **/
        /**************************************************************/
        if ((activation_function != NULL)
        &&  (dropout_factor != 0.0))
        {
            BrainUint j = 0;

            neuron->_sum = 0.0;

            if (neuron->_in)
            {
                for (j = 0; j < neuron->_number_of_input; ++j)
                {
                    neuron->_sum += neuron->_in[j] * neuron->_w[j];
                }
            }

            neuron->_sum += neuron->_bias;

            *(neuron->_out) = activation_function(neuron->_sum) * dropout_factor;
        }
    }

    BRAIN_OUTPUT(activate_neuron)
}

void
delete_neuron(BrainNeuron neuron)
{
    BRAIN_INPUT(delete_neuron)

    if (neuron)
    {
        if (neuron->_w != NULL)
        {
            free(neuron->_w);
        }

        if (neuron->_gradients != NULL)
        {
            free(neuron->_gradients);
        }

        if (neuron->_deltas != NULL)
        {
            free(neuron->_deltas);
        }

        free(neuron);
    }

    BRAIN_OUTPUT(delete_neuron)
}

BrainNeuron
new_neuron(BrainSignal     in,
           const BrainUint number_of_inputs,
           BrainSignal     out,
           BrainSignal     errors)
{
    BRAIN_INPUT(new_neuron)
    BrainNeuron _neuron = NULL;

    if ((out              != NULL)
    &&  (number_of_inputs != 0))
    {
        BrainUint                index   = 0;
        BrainDouble random_value_limit   = (BrainDouble)number_of_inputs;

        _neuron                          = (BrainNeuron)calloc(1, sizeof(Neuron));

        _neuron->_out                    = out;
        _neuron->_number_of_input        = number_of_inputs;
        _neuron->_in                     = in;
        _neuron->_bias                   = get_random_double_value_in_range(-random_value_limit, random_value_limit);
        _neuron->_bias_gradient          = 0.0;
        _neuron->_bias_delta             = 0.0;
        _neuron->_sum                    = 0.0;
        _neuron->_backprop_learning_rate = 1.12;
        _neuron->_rprop_eta_plus         = 1.2;
        _neuron->_rprop_eta_minus        = 0.95;
        _neuron->_rprop_delta_min        = 0.000001;
        _neuron->_rprop_delta_max        = 50;
        _neuron->_activation_function    = activation(Sigmoid);
        _neuron->_derivative_function    = derivative(Sigmoid);
        _neuron->_learning_function      = get_learning_function(BackPropagation);
        _neuron->_errors                 = errors;
        _neuron->_w                      = (BrainSignal)calloc(_neuron->_number_of_input, sizeof(BrainDouble));
        _neuron->_deltas                 = (BrainSignal)calloc(_neuron->_number_of_input, sizeof(BrainDouble));
        _neuron->_gradients              = (BrainSignal)calloc(_neuron->_number_of_input, sizeof(BrainDouble));

        for (index = 0; index < _neuron->_number_of_input; ++index)
        {
            _neuron->_w[index] = get_random_double_value_in_range(-random_value_limit, random_value_limit);
        }
    }

    BRAIN_OUTPUT(new_neuron)

    return _neuron;
}

BrainUint
get_neuron_number_of_input(const BrainNeuron neuron)
{
    if (neuron != NULL)
    {
        return neuron->_number_of_input;
    }

    return 0;
}

BrainDouble
get_neuron_bias(const BrainNeuron neuron)
{
    BrainDouble ret = 0.0;

    if (neuron != NULL)
    {
        ret = neuron->_bias;
    }

    return ret;
}

BrainDouble
get_neuron_weight(const BrainNeuron neuron, const BrainUint index)
{
    BrainDouble ret = 0.0;

    if ((neuron != NULL) &&
        (index < neuron->_number_of_input))
    {
        ret = neuron->_w[index];
    }

    return ret;
}

void
neuron_learning(BrainNeuron neuron, const BrainDouble loss)
{
    if (neuron)
    {
        LearningPtrFunc learning_function = neuron->_learning_function;

        if (learning_function)
        {
            learning_function(neuron, loss);
        }
    }
}

void
deserialize_neuron(BrainNeuron neuron, Context context)
{
    BRAIN_INPUT(deserialize_neuron)

    if (neuron && context)
    {
        const BrainDouble neuron_bias       = node_get_double(context, "bias", 0.0);
        const BrainUint   number_of_weights = get_number_of_node_with_name(context, "weight");
        BrainUint index = 0;

        neuron->_bias = neuron_bias;

        for (index = 0; index < number_of_weights; ++index)
        {
            Context subcontext = get_node_with_name_and_index(context, "weight", index);

            const BrainDouble weight    = node_get_content_as_double(subcontext);

            neuron->_w[index] = weight;
        }
    }

    BRAIN_OUTPUT(deserialize_neuron)
}

void
serialize_neuron(BrainNeuron neuron, Writer writer)
{
    BRAIN_INPUT(serialize_neuron)

    if (neuron && writer)
    {
        const BrainUint number_of_inputs = get_neuron_number_of_input(neuron);

        if (start_element(writer, "neuron"))
        {
            BrainUint index_input = 0;
            BrainChar buffer[50];

            sprintf(buffer, "%lf", neuron->_bias);
            add_attribute(writer, "bias", buffer);

            if (neuron->_w)
            {
                for (index_input = 0;
                     index_input < number_of_inputs;
                     ++index_input)
                {
                    sprintf(buffer, "%lf", neuron->_w[index_input]);
                    write_element(writer, "weight", buffer);
                }
            }

            stop_element(writer);
        }
    }

    BRAIN_OUTPUT(serialize_neuron)
}
