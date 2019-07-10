
#include "brain_neuron.h"
#include "brain_math_utils.h"
#include "brain_random_utils.h"
#include "brain_xml_utils.h"
#include "brain_logging_utils.h"
#include "brain_memory_utils.h"
#include "brain_signal_utils.h"
#include "brain_function_utils.h"
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
    BrainActivationFunction _activation_function;   /*!< Activation function                                  */
    BrainActivationFunction _derivative_function;   /*!< Derivative function                                  */
    /******************************************************************/
    /**                      STRUCTURAL PARAMETERS                   **/
    /******************************************************************/
    BrainSignal       _in;                    /*!< Input signal of an BrainNeuron                       */
    BrainSignal       _w;                     /*!< An array of weight without the bias                  */
    BrainSignal       _gradients;             /*!< weights gradient values                              */
    BrainSignal       _deltas;                /*!< weights delta values                                 */
    BrainSignal       _errors;                /*!< error to correct in the layer                        */
    BrainSignal       _out;                   /*!< An output value pointer owned by the BrainLayer      */
    BrainReal         _sum;                   /*!< Summation of all input time weight                   */
    BrainUint         _number_of_input;       /*!< Number of inputs                                     */
} Neuron;

void
update_neuron(BrainNeuron neuron,
              BrainReal learning_rate,
              BrainReal momentum)
{
    BRAIN_INPUT(update_neuron_using_backpropagation)

    if (BRAIN_ALLOCATED(neuron))
    {
        const BrainUint number_of_inputs      = neuron->_number_of_input;
        BrainReal delta = 0.;
        BrainUint i = 0;
        /******************************************************/
        /**      UPDATE ALL WEIGHT USING GRADIENTS MEANS     **/
        /******************************************************/
        // the bias is the last weight
        for (i = 0; i < number_of_inputs + 1; ++i)
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
set_neuron_activation(BrainNeuron neuron, BrainString name)
{
    BRAIN_INPUT(set_neuron_activation)

    if (BRAIN_ALLOCATED(neuron)
    &&  BRAIN_ALLOCATED(name))
    {
        neuron->_activation_function = brain_activation_function(name);
        neuron->_derivative_function = brain_derivative_function(name);
    }

    BRAIN_OUTPUT(set_neuron_activation)
}

void
activate_neuron(BrainNeuron neuron, const BrainBool is_activated)
{
    BRAIN_INPUT(activate_neuron)

    if (BRAIN_ALLOCATED(neuron))
    {
        BrainActivationFunction activation_function = neuron->_activation_function;

        *(neuron->_out) = 0.;
        neuron->_sum    = 0.;

        /**************************************************************/
        /**                 COMPUTE A(<in, W>)                       **/
        /**************************************************************/
        if ((activation_function != NULL)
        &&  is_activated)
        {
            // Bias si simulated using a dummy 1 input
            neuron->_sum = dot(neuron->_in, neuron->_w, neuron->_number_of_input) + neuron->_w[neuron->_number_of_input];
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
        // Note: You should not forget the bias associated to a dummy 1 input
        BRAIN_NEW(_neuron->_w,              BrainReal, number_of_inputs + 1);
        BRAIN_NEW(_neuron->_deltas,         BrainReal, number_of_inputs + 1);
        BRAIN_NEW(_neuron->_gradients,      BrainReal, number_of_inputs + 1);

        _neuron->_out                    = out;
        _neuron->_number_of_input        = number_of_inputs;
        _neuron->_in                     = in;
        _neuron->_sum                    = 0.;
        _neuron->_activation_function    = brain_activation_function("Sigmoid");
        _neuron->_derivative_function    = brain_derivative_function("Sigmoid");
        _neuron->_errors                 = errors;

        for (index = 0; index < _neuron->_number_of_input; ++index)
        {
            _neuron->_w[index] = (BrainReal)BRAIN_RAND_RANGE(-random_value_limit, random_value_limit);
            _neuron->_deltas[index] = 0.;
        }
        _neuron->_w[index] = (BrainReal)BRAIN_RAND_RANGE(-random_value_limit, random_value_limit);
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
        const BrainUint number_of_weights = get_number_of_node_with_name(context, "weight");
        BrainUint index = 0;

        for (index = 0; index < number_of_weights; ++index)
        {
            Context subcontext = get_node_with_name_and_index(context, "weight", index);

            neuron->_w[index] = (BrainReal)node_get_content_as_double(subcontext);
        }

        neuron->_w[index] = (BrainReal)node_get_double(context, "bias", 0.0);
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

        if (BRAIN_ALLOCATED(neuron->_w))
        {
            if (start_element(writer, "neuron"))
            {
                BrainUint index_input = 0;
                BrainChar buffer[50];

                for (index_input = 0;
                     index_input < number_of_inputs;
                     ++index_input)
                {
                    sprintf(buffer, "%lf", neuron->_w[index_input]);
                    write_element(writer, "weight", buffer);
                }

                sprintf(buffer, "%lf", neuron->_w[index_input]);
                add_attribute(writer, "bias", buffer);

                stop_element(writer);
            }
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
        const BrainUint number_of_inputs            = neuron->_number_of_input;
        BrainActivationFunction derivative_function = neuron->_derivative_function;

        if (BRAIN_ALLOCATED(derivative_function))
        {
            const BrainReal neuron_gradient   = loss * derivative_function(neuron->_sum);
            BrainUint i = 0;
            /******************************************************/
            /**               BACKPROPAGATE $_i                  **/
            /******************************************************/
            for (i = 0; i < number_of_inputs; ++i)
            {
                if (BRAIN_ALLOCATED(neuron->_errors))
                {
                    neuron->_errors[i] += neuron_gradient * neuron->_w[i];
                }

                neuron->_gradients[i] += neuron_gradient * neuron->_in[i];
            }

            // Bias is modelized with a dummy 1 input
            neuron->_gradients[i] += neuron_gradient;
        }
    }

    BRAIN_OUTPUT(backpropagate_neuron_gradient)
}
