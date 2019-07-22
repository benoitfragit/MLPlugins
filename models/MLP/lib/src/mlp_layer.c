#include "mlp_layer.h"
#include "mlp_neuron.h"

#include "brain_xml_utils.h"
#include "brain_logging_utils.h"
#include "brain_random_utils.h"
#include "brain_memory_utils.h"

/**
 * \struct Layer
 * \brief  Internal model for a MLPLayer
 *
 * All protected fields for a MLPLayer
 */
struct Layer
{
    /******************************************************************/
    /**                      STRUCTURAL PARAMETERS                   **/
    /******************************************************************/
    BrainUint       _number_of_neuron; /*!< The number of MLPNeuron    */
    MLPNeuron*      _neurons;          /*!< An array of MLPNeuron      */
    BrainSignal     _in_errors;        /*!< Input vector errors          */
    BrainSignal     _out;              /*!< Output vector of the Layer   */
    BrainRandomMask _mask;             /*!< Dropout activation mask      */
} Layer;

void
set_layer_activation(MLPLayer layer, BrainString name)
{
    BRAIN_INPUT(configure_layer_with_context)

    if (BRAIN_ALLOCATED(layer)
    &&  BRAIN_ALLOCATED(name))
    {
        const BrainUint number_of_neurons = layer->_number_of_neuron;
        BrainUint i = 0;

        for (i = 0; i < number_of_neurons; ++i)
        {
            MLPNeuron neuron = layer->_neurons[i];

            set_neuron_activation(neuron, name);
        }
    }

    BRAIN_OUTPUT(configure_layer_with_context)
}

MLPNeuron
get_layer_neuron(const MLPLayer layer, const BrainUint index)
{
    if (BRAIN_ALLOCATED(layer)
    &&  (index < layer->_number_of_neuron))
    {
        return layer->_neurons[index];
    }

    return NULL;
}

void
delete_layer(MLPLayer layer)
{
    BRAIN_INPUT(delete_layer)

    if (BRAIN_ALLOCATED(layer))
    {
        if (BRAIN_ALLOCATED(layer->_neurons))
        {
            BrainUint i;

            for (i = 0; i < layer->_number_of_neuron; ++i)
            {
                delete_neuron(layer->_neurons[i]);
            }

            BRAIN_DELETE(layer->_neurons);
        }

        delete_random_mask(layer->_mask);

        BRAIN_DELETE(layer->_out);
        BRAIN_DELETE(layer->_in_errors);
        BRAIN_DELETE(layer);
    }

    BRAIN_OUTPUT(delete_layer)
}

MLPLayer
new_layer(const BrainUint     number_of_neurons,
          const BrainUint     number_of_inputs,
          const BrainSignal   in,
          BrainSignal         out_errors)
{
    BRAIN_INPUT(new_layer)
    /******************************************************************/
    /**                       CREATE A NEW LAYER                     **/
    /******************************************************************/
    MLPLayer _layer = NULL;

    if ((number_of_inputs  != 0)
    &&  (number_of_neurons != 0)
    &&  BRAIN_ALLOCATED(in))
    {
        _layer                    = (MLPLayer)calloc(1, sizeof(Layer));
        _layer->_number_of_neuron = number_of_neurons;

        if (0 != _layer->_number_of_neuron)
        {
            BrainUint index = 0;

            BRAIN_NEW(_layer->_neurons, MLPNeuron,_layer->_number_of_neuron);
            BRAIN_NEW(_layer->_out, BrainReal, _layer->_number_of_neuron);
            BRAIN_NEW(_layer->_in_errors, BrainReal, _layer->_number_of_neuron);

            _layer->_mask       = new_random_mask(_layer->_number_of_neuron);

            for (index = 0; (index < _layer->_number_of_neuron); ++index)
            {
                /******************************************************/
                /**              CREATE A NEW NEURON                 **/
                /**                                                  **/
                /** The neuron output is automatically connected to  **/
                /** its parent layer                                 **/
                /**                                                  **/
                /** There are 2 main flows :                         **/
                /**                                                  **/
                /**                  input         output            **/
                /**                -------->      ------->           **/
                /**   PreviousLayer         Neuron        NextLayer  **/
                /**                <--------      <-------           **/
                /**                out_error      in_error           **/
                /******************************************************/
                _layer->_neurons[index] = new_neuron(in,
                                                     number_of_inputs,
                                                     &(_layer->_out[index]),
                                                     out_errors);
            }
        }
    }

    BRAIN_OUTPUT(new_layer)

    return _layer;
}

BrainSignal
get_layer_output(const MLPLayer layer)
{
    BrainSignal ret = NULL;

    if (BRAIN_ALLOCATED(layer))
    {
        ret = layer->_out;
    }

    return ret;
}

BrainUint
get_layer_number_of_neuron(const MLPLayer layer)
{
    BrainUint ret = 0;

    if (BRAIN_ALLOCATED(layer))
    {
        ret = layer->_number_of_neuron;
    }

    return ret;
}

BrainSignal
get_layer_errors(const MLPLayer layer)
{
    BrainSignal ret = NULL;

    if (BRAIN_ALLOCATED(layer))
    {
        ret = layer->_in_errors;
    }

    return ret;
}

void
backpropagate_output_layer(MLPLayer output_layer,
                           const BrainUint number_of_output,
                           const BrainSignal loss)
{
    /******************************************************************/
    /**         BACKPROPAGATE THE ERROR ON THE OUTPUT LAYER          **/
    /**                                                              **/
    /** For the output layer:                                        **/
    /**                                                              **/
    /**                   µ_ji = d C(W) / d w_ji                     **/
    /**                                                              **/
    /** Using the chain rule:                                        **/
    /**                                                              **/
    /**         µ_ji = (d out / d w_ji) * (d C(W) / d out)           **/
    /**                                                              **/
    /** where out is the output of the neuron                        **/
    /** We can easily compute the second member using the            **/
    /** derivative of the network cost function                      **/
    /** Then we could rewrite the first member:                      **/
    /**                                                              **/
    /**     µ_ji = (dA(<in, W>) / d w_ji) * (d C(W) / d out)         **/
    /**                                                              **/
    /** where A is the activation function and <in, W> is the dot    **/
    /** product between the input vector and the weight vector       **/
    /**                                                              **/
    /**    µ_ji = (d<in,W>/dw_ji)*(dA/d<in,W>)*(dC(W)/d out)         **/
    /**                                                              **/
    /** We can easily compute the second member using the            **/
    /** derivative of the activation function                        **/
    /** The first member is the simply in_i, so:                     **/
    /**                                                              **/
    /** µ_ji = in_i * (d A/d <in,W>) * (d C(W)/d out)                **/
    /**                                                              **/
    /** So for the output layer:                                     **/
    /**                                                              **/
    /**              $_ji = (d A/d <in,W>) * (d C(W)/d out)          **/
    /**                                                              **/
    /** the error rate cause by the j th neuron of the previous      **/
    /******************************************************************/
    BRAIN_INPUT(backpropagate_output_layer)

    if (BRAIN_ALLOCATED(output_layer)
    &&  BRAIN_ALLOCATED(loss))
    {
        const BrainUint   number_of_neuron = output_layer->_number_of_neuron;

        if (number_of_neuron == number_of_output)
        {
            BrainUint output_index = 0;

            for (output_index = 0;
                 output_index < number_of_output;
               ++output_index)
            {
                /**************************************************/
                /**           BACKPROPAGATE THE LOSS             **/
                /**************************************************/
                backpropagate_neuron_gradient(output_layer->_neurons[output_index], loss[output_index]);
            }
        }
    }

    BRAIN_OUTPUT(backpropagate_output_layer)
}

void
backpropagate_hidden_layer(MLPLayer hidden_layer)
{
    /******************************************************************/
    /**     BACKPROPAGATE THE ERROR ON THE HIDDEN LAYER              **/
    /**                                                              **/
    /** For the hidden layer, we have :                              **/
    /**                                                              **/
    /** µ_j = in_j * SUM (w_ji * $_ji) * (d A/d <in,W>)              **/
    /**              ----------------                                **/
    /**                     S                                        **/
    /**                                                              **/
    /** Where S denotes the total error cause in the next layer      **/
    /**                                                              **/
    /** During the forward pass, the error cause by a neuron is      **/
    /** spread other all neurons from the next layer                 **/
    /**                                                              **/
    /**                   w1                                         **/
    /**                ---------> next neuron 1 -------->            **/
    /**               /   w2                             \           **/
    /**        neuron-----------> next neuron 2 -------->   E        **/
    /**               \   w3                             /           **/
    /**                ---------> next neuron 3 -------->            **/
    /**                                                              **/
    /** So during the backward pass                                  **/
    /**                 w1 * $_1                                     **/
    /**                <-------- next neuron 1 <--------             **/
    /**               / w2 * $_2                        \            **/
    /**        neuron<---------- next neuron 2 <--------  E          **/
    /**               \ w3 * $_3                        /            **/
    /**                <-------- next neuron 3 <--------             **/
    /******************************************************************/
    BRAIN_INPUT(backpropagate_hidden_layer)

    if (hidden_layer != NULL)
    {
        const BrainUint current_number_of_neuron = hidden_layer->_number_of_neuron;
        BrainUint i = 0;

        for (i = 0; i < current_number_of_neuron; ++i)
        {
            const BrainReal loss = hidden_layer->_in_errors[i];
            const BrainBool activated = get_random_state(hidden_layer->_mask, i);

            if (activated)
            {
                backpropagate_neuron_gradient(hidden_layer->_neurons[i], loss);
            }
        }
    }
    BRAIN_OUTPUT(backpropagate_hidden_layer)
}

void
activate_layer(MLPLayer layer, const BrainBool hidden_layer)
{
    BRAIN_INPUT(activate_layer)
    if (BRAIN_ALLOCATED(layer))
    {
        BrainUint i = 0;

        BRAIN_SET(layer->_in_errors, 0, BrainReal, layer->_number_of_neuron);

        if (hidden_layer)
        {
            generate_random_mask(layer->_mask);
        }
        else
        {
            // if dropout is not activated on this layer
            // thus all neurons will be activated
            generate_unit_mask(layer->_mask);
        }

        for (i = 0; i < layer->_number_of_neuron; ++i)
        {
            const BrainBool activation = get_random_state(layer->_mask, i);

            /**********************************************************/
            /**                    ACTIVATE ALL NEURONS              **/
            /**********************************************************/
            activate_neuron(layer->_neurons[i], !hidden_layer || activation);
        }
    }
    BRAIN_OUTPUT(activate_layer)
}

void
serialize_layer(MLPLayer layer, Writer writer)
{
    BRAIN_INPUT(serialize_layer)
    if (BRAIN_ALLOCATED(writer)
    &&  BRAIN_ALLOCATED(layer))
    {
        if (start_element(writer, "layer"))
        {
            const BrainUint number_of_neurons = layer->_number_of_neuron;
            BrainUint i = 0;

            for (i = 0; i < number_of_neurons; ++i)
            {
                const MLPNeuron neuron = layer->_neurons[i];

                serialize_neuron(neuron, writer);
            }

            stop_element(writer);
        }
    }
    BRAIN_OUTPUT(serialize_layer)
}

void
deserialize_layer(MLPLayer layer, Context context)
{
    BRAIN_INPUT(deserialize_layer)
    if (BRAIN_ALLOCATED(layer) &&
        BRAIN_ALLOCATED(context))
    {
        const BrainUint number_of_serialized_neurons = get_number_of_node_with_name(context, "neuron");
        const BrainUint number_of_neurons = layer->_number_of_neuron;

        if (number_of_neurons == number_of_serialized_neurons)
        {
            BrainUint i = 0;

            for (i = 0; i < number_of_neurons; ++i)
            {
                Context neuron_context = get_node_with_name_and_index(context, "neuron", i);
                MLPNeuron neuron = layer->_neurons[i];

                deserialize_neuron(neuron, neuron_context);
            }
        }
    }
    BRAIN_OUTPUT(deserialize_layer)
}

void
update_layer(MLPLayer layer,
             BrainReal learning_rate,
             BrainReal momentum)
{
    BRAIN_INPUT(update_layer)

    if (BRAIN_ALLOCATED(layer))
    {
        BrainUint i = 0;
        for (i = 0; i < layer->_number_of_neuron; ++i)
        {
            /**********************************************************/
            /**                    UPDATE ALL NEURONS                **/
            /**********************************************************/
            update_neuron(layer->_neurons[i],
                          learning_rate,
                          momentum);
        }
    }

    BRAIN_OUTPUT(update_layer)
}
