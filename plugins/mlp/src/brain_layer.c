#include "brain_layer.h"
#include "brain_neuron.h"
#include "brain_cost.h"
#include "brain_xml_utils.h"
#include "brain_logging_utils.h"
#include "brain_random.h"

#define BRAIN_MASK_SIZE 32
#define BRAIN_ACTIVATION(mask,j,k) ((mask[j] >> k) & 0x00000001)

/**
 * \struct Layer
 * \brief  Internal model for a BrainLayer
 *
 * All protected fields for a BrainLayer
 */
struct Layer
{
    /******************************************************************/
    /**                      STRUCTURAL PARAMETERS                   **/
    /******************************************************************/
    BrainUint    _number_of_neuron; /*!< The number of BrainNeuron    */
    BrainUint    _mask_size;        /*!< The dropout mask size        */
    BrainNeuron* _neurons;          /*!< An array of BrainNeuron      */
    BrainSignal  _in_errors;        /*!< Input vector errors          */
    BrainSignal  _out;              /*!< Output vector of the Layer   */
    BrainUint*   _mask;             /*!< Dropout activation mask      */
    BrainUint*   _mask_node;        /*!< Dropout activation mask      */
    BrainUint*   _mask_slot;        /*!< Dropout activation mask      */
    /******************************************************************/
    /**                      FUNCTIONAL PARAMETERS                   **/
    /******************************************************************/
    CostPtrFunc  _cost_function_derivative; /*!< Cost function derivative function */
} Layer;

void
configure_layer_with_context(BrainLayer layer, Context context)
{
    BRAIN_INPUT(configure_layer_with_context)

    if (layer && context)
    {
        const BrainUint number_of_neurons = layer->_number_of_neuron;
        BrainUint i = 0;

        BrainChar* buffer = (BrainChar *)node_get_prop(context, "cost-function");
        BrainCostFunctionType cost_function_type = get_cost_function_type(buffer);

        if (buffer != NULL)
        {
            free(buffer);
        }

        layer->_cost_function_derivative = get_cost_function_derivative(cost_function_type);

        for (i = 0; i < number_of_neurons; ++i)
        {
            BrainNeuron neuron = layer->_neurons[i];

            configure_neuron_with_context(neuron, context);
        }
    }

    BRAIN_OUTPUT(configure_layer_with_context)
}

BrainNeuron
get_layer_neuron(const BrainLayer layer, const BrainUint index)
{
    if ((layer != NULL)
    &&  (index < layer->_number_of_neuron))
    {
        return layer->_neurons[index];
    }

    return NULL;
}

void
delete_layer(BrainLayer layer)
{
    BRAIN_INPUT(delete_layer)

    if (layer != NULL)
    {
        if (layer->_neurons)
        {
            BrainUint i;

            for (i = 0; i < layer->_number_of_neuron; ++i)
            {
                delete_neuron(layer->_neurons[i]);
            }

            free(layer->_neurons);
        }

        if (layer->_out)
        {
            free(layer->_out);
        }

        if (layer->_in_errors)
        {
            free(layer->_in_errors);
        }

        if (layer->_mask_node)
        {
            free(layer->_mask_node);
        }

        if (layer->_mask_slot)
        {
            free(layer->_mask_slot);
        }

        if (layer->_mask)
        {
            free(layer->_mask);
        }

        free(layer);
    }

    BRAIN_OUTPUT(delete_layer)
}

BrainLayer
new_layer(const BrainUint     number_of_neurons,
          const BrainUint     number_of_inputs,
          const BrainSignal   in,
          BrainSignal         out_errors)
{
    BRAIN_INPUT(new_layer)
    /******************************************************************/
    /**                       CREATE A NEW LAYER                     **/
    /******************************************************************/
    BrainLayer _layer = NULL;

    if ((number_of_inputs  != 0)
    &&  (number_of_neurons != 0)
    &&  (in                != NULL))
    {
        _layer                    = (BrainLayer)calloc(1, sizeof(Layer));
        _layer->_number_of_neuron = number_of_neurons;
        /**************************************************************/
        /** THIS THE COST FUNCTION DERIVATIVE USED TO COMPUTE THE    **/
        /** BACKPROPAGATION ALGORITHM                                **/
        /**************************************************************/
        _layer->_cost_function_derivative = get_cost_function_derivative(Quadratic);

        if (0 != _layer->_number_of_neuron)
        {
            BrainUint index = 0;

           _layer->_mask_size  = (BrainUint)(_layer->_number_of_neuron / BRAIN_MASK_SIZE);
            if (_layer->_mask_size == 0)
            {
                _layer->_mask_size = 1;
            }
            else
            {
                if ((_layer->_number_of_neuron - _layer->_mask_size * BRAIN_MASK_SIZE) >= 1)
                {
                    ++_layer->_mask_size;
                }
            }

            _layer->_neurons    = (BrainNeuron*)calloc(_layer->_number_of_neuron,   sizeof(BrainNeuron));
            _layer->_out        = (BrainSignal) calloc(_layer->_number_of_neuron,   sizeof(BrainReal));
            _layer->_in_errors  = (BrainSignal) calloc(_layer->_number_of_neuron,   sizeof(BrainReal));
            _layer->_mask       = (BrainUint *) calloc(_layer->_mask_size,          sizeof(BrainUint));
            _layer->_mask_node  = (BrainUint *) calloc(_layer->_number_of_neuron,   sizeof(BrainUint));
            _layer->_mask_slot  = (BrainUint *) calloc(_layer->_number_of_neuron,   sizeof(BrainUint));

            for (index = 0;
                 (index < _layer->_number_of_neuron);
                 ++index)
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

                _layer->_mask_node[index] = index / BRAIN_MASK_SIZE;
                _layer->_mask_slot[index] = index % BRAIN_MASK_SIZE;
            }
        }
    }

    BRAIN_OUTPUT(new_layer)

    return _layer;
}

BrainSignal
get_layer_output(const BrainLayer layer)
{
    if (layer != NULL)
    {
        return layer->_out;
    }

    return NULL;
}

BrainUint
get_layer_number_of_neuron(const BrainLayer layer)
{
    if (layer)
    {
        return layer->_number_of_neuron;
    }

    return 0;
}

BrainSignal
get_layer_errors(const BrainLayer layer)
{
    BrainSignal ret = NULL;

    if (layer != NULL)
    {
        ret = layer->_in_errors;
    }

    return ret;
}

void
backpropagate_output_layer(BrainLayer output_layer,
                           const BrainUint number_of_output,
                           const BrainSignal desired)
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

    if ((output_layer != NULL)
    &&  (desired != NULL))
    {
        const BrainSignal output           = get_layer_output(output_layer);
        const BrainUint   number_of_neuron = output_layer->_number_of_neuron;

        if (number_of_neuron == number_of_output)
        {
            CostPtrFunc cost_function_derivative = output_layer->_cost_function_derivative;

            BrainUint output_index = 0;

            for (output_index = 0;
                 output_index < number_of_output;
               ++output_index)
            {
                /******************************************************/
                /**            COMPUTE THE COST DERIVATIVE           **/
                /******************************************************/
                const BrainReal loss = cost_function_derivative(output[output_index], desired[output_index]);
                /**************************************************/
                /**           CALL LEARNING FUNCTION             **/
                /**************************************************/
                neuron_learning(output_layer->_neurons[output_index], loss);
            }
        }
    }

    BRAIN_OUTPUT(backpropagate_output_layer)
}

void
backpropagate_hidden_layer(BrainLayer hidden_layer)
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
        const BrainUint* mask_node = hidden_layer->_mask_node;
        const BrainUint* mask_slot = hidden_layer->_mask_slot;
        BrainUint i = 0;

        for (i = 0; i < current_number_of_neuron; ++i)
        {
            BrainNeuron current_neuron = hidden_layer->_neurons[i];

            if (current_neuron != NULL)
            {
                const BrainReal loss = hidden_layer->_in_errors[i];
                const BrainUint j = mask_node[i];
                const BrainUint k = mask_slot[i];
                const BrainBool activated = BRAIN_ACTIVATION(hidden_layer->_mask, j, k);

                if (activated)
                {
                    neuron_learning(current_neuron, loss);
                }
            }
        }
    }
    BRAIN_OUTPUT(backpropagate_hidden_layer)
}

void
activate_layer(BrainLayer layer, const BrainBool hidden_layer)
{
    BRAIN_INPUT(activate_layer)
    if (layer != NULL)
    {
        const BrainUint* mask_node = layer->_mask_node;
        const BrainUint* mask_slot = layer->_mask_slot;
        BrainUint i = 0;

        memset(layer->_in_errors, 0, layer->_number_of_neuron * sizeof(BrainReal));

        if (hidden_layer)
        {
            for (i = 0; i < layer->_mask_size; ++i)
            {
                layer->_mask[i] = (BrainUint)BRAIN_RAND_RANGE(0, UINT_MAX);
            }
        }
        else
        {
            // all neurons will be activated
            memset(layer->_mask, UINT_MAX, layer->_mask_size * sizeof(BrainUint));
        }

        for (i = 0; i < layer->_number_of_neuron; ++i)
        {
            const BrainUint j = mask_node[i];
            const BrainUint k = mask_slot[i];
            const BrainBool activation = BRAIN_ACTIVATION(layer->_mask, j, k);
            /**********************************************************/
            /**                    ACTIVATE ALL NEURONS              **/
            /**********************************************************/
            activate_neuron(layer->_neurons[i], activation);
        }
    }
    BRAIN_OUTPUT(activate_layer)
}

void
serialize_layer(BrainLayer layer, Writer writer)
{
    BRAIN_INPUT(serialize_layer)
    if (writer && layer)
    {
        if (start_element(writer, "layer"))
        {
            const BrainUint number_of_neurons = layer->_number_of_neuron;
            BrainUint i = 0;

            for (i = 0; i < number_of_neurons; ++i)
            {
                const BrainNeuron neuron = layer->_neurons[i];

                serialize_neuron(neuron, writer);
            }

            stop_element(writer);
        }
    }
    BRAIN_OUTPUT(serialize_layer)
}

void
deserialize_layer(BrainLayer layer, Context context)
{
    BRAIN_INPUT(deserialize_layer)
    if (layer && context)
    {
        const BrainUint number_of_serialized_neurons = get_number_of_node_with_name(context, "neuron");
        const BrainUint number_of_neurons = layer->_number_of_neuron;

        if (number_of_neurons == number_of_serialized_neurons)
        {
            BrainUint i = 0;

            for (i = 0; i < number_of_neurons; ++i)
            {
                Context neuron_context = get_node_with_name_and_index(context, "neuron", i);
                BrainNeuron neuron = layer->_neurons[i];

                deserialize_neuron(neuron, neuron_context);
            }
        }
    }
    BRAIN_OUTPUT(deserialize_layer)
}
