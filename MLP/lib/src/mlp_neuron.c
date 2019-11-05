#include "mlp_neuron.h"

#include "brain_math_utils.h"
#include "brain_random_utils.h"
#include "brain_xml_utils.h"
#include "brain_logging_utils.h"
#include "brain_memory_utils.h"
#include "brain_signal_utils.h"
#include "brain_function_utils.h"
#include "brain_weight_utils.h"
/**
 * \struct Neuron
 * \brief  Internal model for a MLPNeuron
 *
 * All protected fields for a MLPNeuron
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
    BrainWeight*      _w;                     /*!< An array of weight                                   */
    BrainSignal       _in;                    /*!< Input signal of an MLPNeuron                       */
    BrainSignal       _errors;                /*!< error to correct in the layer                        */
    BrainSignal       _out;                   /*!< An output value pointer owned by the MLPLayer      */
    BrainReal         _sum;                   /*!< Summation of all input time weight                   */
    BrainUint         _number_of_input;       /*!< Number of inputs                                     */
} Neuron;

void
update_neuron(MLPNeuron neuron,
              BrainReal learning_rate,
              BrainReal momentum)
{
    BRAIN_INPUT(update_neuron_using_backpropagation)

    if (BRAIN_ALLOCATED(neuron))
    {
        const BrainUint number_of_inputs  = neuron->_number_of_input;
        BrainUint i = 0;
        /******************************************************/
        /**      UPDATE ALL WEIGHT USING GRADIENTS MEANS     **/
        /******************************************************/
        for (i = 0; i < number_of_inputs + 1; ++i)
        {
            update_weight(neuron->_w[i], learning_rate, momentum);
        }
    }

    BRAIN_OUTPUT(update_neuron_using_backpropagation)
}

void
activate_neuron(MLPNeuron neuron, const BrainBool is_activated)
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
            BrainUint i = 0;
            BrainReal w = 0.;

#if defined(__GNUC__)
            #pragma GCC ivdep
#endif
            for (i = 0; i < neuron->_number_of_input; ++i)
            {
                w = get_weight(neuron->_w[i]);
                neuron->_sum += w * neuron->_in[i];
            }
            w = get_weight(neuron->_w[i]);
            neuron->_sum += w * 1.0;

            *(neuron->_out) = activation_function(neuron->_sum);
        }
    }

    BRAIN_OUTPUT(activate_neuron)
}

void
delete_neuron(MLPNeuron neuron)
{
    BRAIN_INPUT(delete_neuron)

    if (BRAIN_ALLOCATED(neuron))
    {
        BrainUint i = 0;
        for (i = 0; i < neuron->_number_of_input + 1; ++i)
        {
            BRAIN_DELETE(neuron->_w[i]);
        }
        BRAIN_DELETE(neuron->_w);
        BRAIN_DELETE(neuron);
    }

    BRAIN_OUTPUT(delete_neuron)
}

MLPNeuron
new_neuron(BrainActivationFunction activation_function,
           BrainActivationFunction derivative_function,
           BrainSignal     in,
           const BrainUint number_of_inputs,
           BrainSignal     out,
           BrainSignal     errors)
{
    BRAIN_INPUT(new_neuron)
    MLPNeuron _neuron = NULL;

    if (BRAIN_ALLOCATED(out)
    &&  (0 < number_of_inputs))
    {
        BrainUint                index   = 0;
        BrainReal random_value_limit     = 1./sqrt((BrainReal)number_of_inputs);

        BRAIN_NEW(_neuron,                  Neuron,    1);
        // Note: You should not forget the bias associated to a dummy 1 input
        BRAIN_NEW(_neuron->_w,              BrainWeight, number_of_inputs + 1);

        _neuron->_out                    = out;
        _neuron->_number_of_input        = number_of_inputs;
        _neuron->_in                     = in;
        _neuron->_sum                    = 0.;
        _neuron->_activation_function    = activation_function;
        _neuron->_derivative_function    = derivative_function;
        _neuron->_errors                 = errors;

        for (index = 0; index < _neuron->_number_of_input + 1; ++index)
        {
            _neuron->_w[index] = new_weight(random_value_limit);
        }
    }

    BRAIN_OUTPUT(new_neuron)

    return _neuron;
}

BrainUint
get_neuron_number_of_input(const MLPNeuron neuron)
{
    BrainUint ret = 0;
    if (BRAIN_ALLOCATED(neuron))
    {
        ret = neuron->_number_of_input;
    }

    return ret;
}

BrainReal
get_neuron_weight(const MLPNeuron neuron, const BrainUint index)
{
    BrainReal ret = 0.0;

    if (BRAIN_ALLOCATED(neuron) &&
        (index < neuron->_number_of_input))
    {
        ret = get_weight(neuron->_w[index]);
    }

    return ret;
}

void
deserialize_neuron(MLPNeuron neuron, Context context)
{
    BRAIN_INPUT(deserialize_neuron)

    if (BRAIN_ALLOCATED(neuron) &&
        BRAIN_ALLOCATED(context))
    {
        const BrainUint number_of_weights = get_number_of_node_with_name(context, "weight");
        BrainUint index = 0;
        BrainReal value = 0.;

        for (index = 0; index < number_of_weights; ++index)
        {
            Context subcontext = get_node_with_name_and_index(context, "weight", index);
            value = (BrainReal)node_get_content_as_double(subcontext);
            set_weight(neuron->_w[index], value);
        }

        value = (BrainReal)node_get_double(context, "bias", 0.0);
        set_weight(neuron->_w[neuron->_number_of_input], value);
    }

    BRAIN_OUTPUT(deserialize_neuron)
}

void
serialize_neuron(MLPNeuron neuron, Writer writer)
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
                BrainReal w = 0.;

                w = get_weight(neuron->_w[number_of_inputs]);
                sprintf(buffer, "%lf", w);
                add_attribute(writer, "bias", buffer);

                for (index_input = 0;
                     index_input < number_of_inputs;
                     ++index_input)
                {
                    w = get_weight(neuron->_w[index_input]);
                    sprintf(buffer, "%lf", w);
                    write_element(writer, "weight", buffer);
                }

                stop_element(writer);
            }
        }
    }

    BRAIN_OUTPUT(serialize_neuron)
}

void
backpropagate_neuron_gradient(MLPNeuron neuron, const BrainReal loss)
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
                    BrainReal w = get_weight(neuron->_w[i]);
                    neuron->_errors[i] += neuron_gradient * w;
                }

                accumulate_gradient(neuron->_w[i], neuron_gradient, neuron->_in[i]);
            }

            // Bias is modelized with a dummy 1 input
            accumulate_gradient(neuron->_w[i], neuron_gradient, 1.);
        }
    }

    BRAIN_OUTPUT(backpropagate_neuron_gradient)
}
