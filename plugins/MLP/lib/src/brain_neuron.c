
#include "brain_neuron.h"
#include "brain_math_utils.h"
#include "brain_random_utils.h"
#include "brain_xml_utils.h"
#include "brain_logging_utils.h"
#include "brain_memory_utils.h"
#include "brain_enum_utils.h"
#include "brain_signal_utils.h"
/**
 * \enum BrainActivationType
 * \brief enumeration to choose neurons activation function
 */
typedef enum BrainActivationType
{
    Identity,           /*!< Identity activation */
    Sigmoid,            /*!< sigmoid activation  */
    TanH,               /*!< tanh activation     */
    ArcTan,             /*!< arctan activation   */
    SoftPlus,           /*!< softplus activation */
    Sinusoid,           /*!< sinusoid activation */
    ReLu,               /*!< relu activation     */
    Invalid_Activation, /*!< Invalid activation  */

    First_Activation = Identity,
    Last_Activation = Invalid_Activation
} BrainActivationType;
/**
 * \brief function pointer on an activation function
 *
 * It let neurons use several activation function and
 * automatically compute the activation an it's derivation
 *
 * \param value dot product of input vector and weight vector of a neuron
 * \return the value of the activation
 */
typedef BrainReal (*ActivationPtrFunc)(const BrainReal value);
/**
 * \struct Neuron
 * \brief  Internal model for a BrainNeuron
 *
 * All protected fields for a BrainNeuron
 */
typedef struct Neuron
{
    /******************************************************************/
    /**                      FUNCTIONAL PARAMETERS                   **/
    /******************************************************************/
    ActivationPtrFunc _activation_function;   /*!< Activation function                                  */
    ActivationPtrFunc _derivative_function;   /*!< Derivative function                                  */
    /******************************************************************/
    /**                      STRUCTURAL PARAMETERS                   **/
    /******************************************************************/
    BrainSignal       _in;                    /*!< Input signal of an BrainNeuron                       */
    BrainSignal       _w;                     /*!< An array of weight without the bias                  */
    BrainSignal       _gradients;             /*!< weights gradient values                              */
    BrainSignal       _deltas;                /*!< weights delta values                                 */
    BrainSignal       _errors;                /*!< error to correct in the layer                        */
    BrainSignal       _out;                   /*!< An output value pointer owned by the BrainLayer      */
    BrainReal         _bias;                  /*!< Bias of the neuron                                   */
    BrainReal         _bias_delta;            /*!< Bias delta                                           */
    BrainReal         _bias_gradient;         /*!< Bias gradient                                        */
    BrainReal         _sum;                   /*!< Summation of all input time weight                   */
    /******************************************************************/
    /**                        TRAINING PARAMETERS                   **/
    /******************************************************************/
    BrainReal         _learning_rate;         /*!< BackProp learning rate                               */
    BrainReal         _momemtum;              /*!< BackProp momentum value                              */
    BrainUint         _number_of_input;       /*!< Number of inputs                                     */
} Neuron;

static BrainString activation_name[] = {
    "Identity",
    "Sigmoid",
    "TanH",
    "ArcTan",
    "SoftPlus",
    "Sinus",
    "ReLu"
};

static ActivationPtrFunc _activation_functions[][2] = {{identity,           identity_derivative},
                                                       {sigmoid,            sigmoid_derivative},
                                                       {tangeant_hyperbolic,tangeant_hyperbolic_derivative},
                                                       {co_tangeant,        co_tangeant_derivative},
                                                       {softplus,           softplus_derivative},
                                                       {sinusoid,           sinusoid_derivative},
                                                       {relu,               relu_derivative}};

void
update_neuron(BrainNeuron neuron, const BrainReal minibatch_size)
{
    BRAIN_INPUT(update_neuron_using_backpropagation)

    if (BRAIN_ALLOCATED(neuron))
    {
        const BrainUint number_of_inputs      = neuron->_number_of_input;
        const BrainReal learning_rate         = neuron->_learning_rate / (BrainReal)minibatch_size;
        const BrainReal momentum              = neuron->_momemtum;
        BrainReal delta = 0.;
        BrainUint i = 0;
        /******************************************************/
        /**      UPDATE ALL WEIGHT USING GRADIENTS MEANS     **/
        /******************************************************/
        delta = learning_rate * neuron->_bias_gradient - momentum * neuron->_bias_delta;
        neuron->_bias -= delta;
        neuron->_bias_gradient = 0.;
        neuron->_bias_delta = delta;

        for (i = 0; i < number_of_inputs; ++i)
        {
            delta = learning_rate * neuron->_gradients[i] - momentum * neuron->_deltas[i];
            neuron->_w[i] -= delta;
            neuron->_gradients[i] = 0.;
            neuron->_deltas[i] = delta;
        }
    }

    BRAIN_OUTPUT(update_neuron_using_backpropagation)
}

void
configure_neuron_with_context(BrainNeuron neuron, Context context)
{
    BRAIN_INPUT(configure_neuron_with_context)

    if (BRAIN_ALLOCATED(neuron)
    &&  BRAIN_ALLOCATED(context))
    {
        BrainChar* buffer = NULL;
        BrainActivationType activation_type = Sigmoid;

        neuron->_learning_rate       = (BrainReal)node_get_double(context, "learning-rate", 0.005);
        neuron->_momemtum            = (BrainReal)node_get_double(context, "momentum", 0.001);
        buffer                       = (BrainChar *)node_get_prop(context, "activation-function");
        activation_type              = get_enum_values(activation_name, First_Activation, Last_Activation, buffer);
        neuron->_activation_function = _activation_functions[activation_type][Function];
        neuron->_derivative_function = _activation_functions[activation_type][Derivative];

        BRAIN_DELETE(buffer);
    }

    BRAIN_OUTPUT(configure_neuron_with_context)
}

void
activate_neuron(BrainNeuron neuron, const BrainBool is_activated)
{
    BRAIN_INPUT(activate_neuron)

    if (BRAIN_ALLOCATED(neuron))
    {
        ActivationPtrFunc activation_function = neuron->_activation_function;

        *(neuron->_out) = 0.;
        neuron->_sum    = 0.;

        /**************************************************************/
        /**                 COMPUTE A(<in, W>)                       **/
        /**************************************************************/
        if ((activation_function != NULL)
        &&  is_activated)
        {
            neuron->_sum = dot(neuron->_in, neuron->_w, neuron->_number_of_input) + neuron->_bias;
            *(neuron->_out) = activation_function(neuron->_sum);
        }
    }

    BRAIN_OUTPUT(activate_neuron)
}

void
delete_neuron(BrainNeuron neuron)
{
    BRAIN_INPUT(delete_neuron)

    if (BRAIN_ALLOCATED(neuron))
    {
        BRAIN_DELETE(neuron->_w);
        BRAIN_DELETE(neuron->_gradients);
        BRAIN_DELETE(neuron->_deltas);
        BRAIN_DELETE(neuron);
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

    if (BRAIN_ALLOCATED(out)
    &&  (0 < number_of_inputs))
    {
        BrainUint                index   = 0;
        BrainReal random_value_limit     = 1./sqrt((BrainReal)number_of_inputs);

        BRAIN_NEW(_neuron,                  Neuron,    1);
        BRAIN_NEW(_neuron->_w,              BrainReal, number_of_inputs);
        BRAIN_NEW(_neuron->_deltas,         BrainReal, number_of_inputs);
        BRAIN_NEW(_neuron->_gradients,      BrainReal, number_of_inputs);

        _neuron->_out                    = out;
        _neuron->_number_of_input        = number_of_inputs;
        _neuron->_in                     = in;
        _neuron->_bias                   = (BrainReal)BRAIN_RAND_RANGE(-random_value_limit, random_value_limit);
        _neuron->_bias_gradient          = 0.;
        _neuron->_bias_delta             = 0.;
        _neuron->_sum                    = 0.;
        _neuron->_learning_rate          = 1.12;
        _neuron->_momemtum               = 0.0;
        _neuron->_activation_function    = _activation_functions[Sigmoid][Function];
        _neuron->_derivative_function    = _activation_functions[Sigmoid][Derivative];
        _neuron->_errors                 = errors;

        for (index = 0; index < _neuron->_number_of_input; ++index)
        {
            _neuron->_w[index] = (BrainReal)BRAIN_RAND_RANGE(-random_value_limit, random_value_limit);
            _neuron->_deltas[index] = 0.;
        }
    }

    BRAIN_OUTPUT(new_neuron)

    return _neuron;
}

BrainUint
get_neuron_number_of_input(const BrainNeuron neuron)
{
    BrainUint ret = 0;
    if (BRAIN_ALLOCATED(neuron))
    {
        ret = neuron->_number_of_input;
    }

    return ret;
}

BrainReal
get_neuron_bias(const BrainNeuron neuron)
{
    BrainReal ret = 0.;

    if (BRAIN_ALLOCATED(neuron))
    {
        ret = neuron->_bias;
    }

    return ret;
}

BrainReal
get_neuron_weight(const BrainNeuron neuron, const BrainUint index)
{
    BrainReal ret = 0.0;

    if (BRAIN_ALLOCATED(neuron) &&
        (index < neuron->_number_of_input))
    {
        ret = neuron->_w[index];
    }

    return ret;
}

void
deserialize_neuron(BrainNeuron neuron, Context context)
{
    BRAIN_INPUT(deserialize_neuron)

    if (BRAIN_ALLOCATED(neuron) &&
        BRAIN_ALLOCATED(context))
    {
        const BrainReal neuron_bias       = (BrainReal)node_get_double(context, "bias", 0.0);
        const BrainUint number_of_weights = get_number_of_node_with_name(context, "weight");
        BrainUint index = 0;

        neuron->_bias = neuron_bias;

        for (index = 0; index < number_of_weights; ++index)
        {
            Context subcontext = get_node_with_name_and_index(context, "weight", index);

            neuron->_w[index] = (BrainReal)node_get_content_as_double(subcontext);
        }
    }

    BRAIN_OUTPUT(deserialize_neuron)
}

void
serialize_neuron(BrainNeuron neuron, Writer writer)
{
    BRAIN_INPUT(serialize_neuron)

    if (BRAIN_ALLOCATED(neuron)
    &&  BRAIN_ALLOCATED(writer))
    {
        const BrainUint number_of_inputs = get_neuron_number_of_input(neuron);

        if (start_element(writer, "neuron"))
        {
            BrainUint index_input = 0;
            BrainChar buffer[50];

            sprintf(buffer, "%lf", neuron->_bias);
            add_attribute(writer, "bias", buffer);

            if (BRAIN_ALLOCATED(neuron->_w))
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

void
backpropagate_neuron_gradient(BrainNeuron neuron, const BrainReal loss)
{
    BRAIN_INPUT(backpropagate_neuron_gradient)

    if (BRAIN_ALLOCATED(neuron))
    {
        const BrainUint number_of_inputs      = neuron->_number_of_input;
        ActivationPtrFunc derivative_function = neuron->_derivative_function;

        if (BRAIN_ALLOCATED(derivative_function))
        {
            const BrainReal neuron_gradient   = loss * derivative_function(neuron->_sum);
            BrainUint i = 0;
            /******************************************************/
            /**               BACKPROPAGATE $_i                  **/
            /******************************************************/
            neuron->_bias_gradient += neuron_gradient;

            for (i = 0; i < number_of_inputs; ++i)
            {
                if (BRAIN_ALLOCATED(neuron->_errors))
                {
                    neuron->_errors[i] += neuron_gradient * neuron->_w[i];
                }

                neuron->_gradients[i] += neuron_gradient * neuron->_in[i];
            }
        }
    }

    BRAIN_OUTPUT(backpropagate_neuron_gradient)
}
