#include "brain_layer.h"
#include "brain_neuron.h"
#include "brain_cost.h"

/**
 * \struct Layer
 * \brief  Internal model for a BrainLayer
 *
 * All protected fields for a BrainLayer
 */
struct Layer
{
    BrainUint    _number_of_neuron; /*!< The number of BrainNeuron         */
    BrainNeuron* _neurons;          /*!< An array of BrainNeuron           */

    BrainSignal  _in;               /*!< Input vector of the layer         */
    BrainSignal  _in_errors;        /*!< Input vector errors               */

    BrainSignal  _out;              /*!< Output vector of the Layer        */
    BrainSignal  _out_errors;       /*!< Output vector errors              */

    CostPtrFunc  _cost_function_derivative;
} Layer;

void
set_layer_parameters(BrainLayer layer,
                     const BrainActivationType   activation_type,
                     const BrainCostFunctionType costfunction_type,
                     const BrainBool             use_dropout,
                     const BrainDouble           dropout_factor,
                     const BrainLearningType     learning_type,
                     const BrainDouble           backpropagation_learning_rate,
                     const BrainDouble           resilient_delta_min,
                     const BrainDouble           resilient_delta_max,
                     const BrainDouble           resilient_eta_positive,
                     const BrainDouble           resilient_eta_negative)
{
    if (layer)
    {
        const BrainUint number_of_neurons = layer->_number_of_neuron;
        BrainUint i = 0;

        layer->_cost_function_derivative = get_cost_function_derivative(costfunction_type);

        for (i = 0; i < number_of_neurons; ++i)
        {
            BrainNeuron neuron = layer->_neurons[i];

            set_neuron_parameters(neuron,
                                    activation_type,
                                    use_dropout,
                                    dropout_factor,
                                    learning_type,
                                    backpropagation_learning_rate,
                                    resilient_delta_min,
                                    resilient_delta_max,
                                    resilient_eta_positive,
                                    resilient_eta_negative);
        }
    }
}

BrainNeuron
get_layer_neuron(const BrainLayer layer,
                 const BrainUint index)
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

        free(layer);
    }
}

BrainLayer
new_layer(const BrainUint     number_of_neurons,
          const BrainUint     number_of_inputs,
          const BrainSignal   in,
          BrainSignal         out_errors)
{
    BrainLayer _layer = NULL;

    if ((number_of_inputs  != 0)
    &&  (number_of_neurons != 0)
    &&  (in                != NULL))
    {
        _layer                    = (BrainLayer)calloc(1, sizeof(Layer));
        _layer->_number_of_neuron = number_of_neurons;
        _layer->_in               = in;
        _layer->_out_errors       = out_errors;

        _layer->_cost_function_derivative = get_cost_function_derivative(Quadratic);

        if (0 != _layer->_number_of_neuron)
        {
            BrainUint index = 0;

            _layer->_neurons    = (BrainNeuron *)calloc(_layer->_number_of_neuron, sizeof(BrainNeuron));
            _layer->_out        = (BrainSignal  )calloc(_layer->_number_of_neuron, sizeof(BrainDouble));
            _layer->_in_errors  = (BrainSignal)calloc(_layer->_number_of_neuron, sizeof(BrainDouble));

            for (index = 0;
                 (index < _layer->_number_of_neuron);
                 ++index)
            {
                _layer->_neurons[index] = new_neuron(number_of_inputs,
                                                     &(_layer->_out[index]),
                                                     out_errors);
            }
        }
    }

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
                const BrainDouble loss = cost_function_derivative(output[output_index], desired[output_index]);

                BrainNeuron output_neuron = get_layer_neuron(output_layer, output_index);

                if (output_neuron != NULL)
                {
                    neuron_learning(output_neuron, loss);
                }
            }
        }
    }
}

void
backpropagate_hidden_layer(BrainLayer hidden_layer)
{
    if (hidden_layer != NULL)
    {
        const BrainUint current_number_of_neuron = hidden_layer->_number_of_neuron;

        BrainUint i = 0;

        for (i = 0; i < current_number_of_neuron; ++i)
        {
            BrainNeuron current_neuron = hidden_layer->_neurons[i];

            if (current_neuron != NULL)
            {
                const BrainDouble loss = hidden_layer->_in_errors[i];

                neuron_learning(current_neuron, loss);
            }
        }
    }
}

void
apply_layer_correction(BrainLayer layer)
{
    if (layer != NULL)
    {
        const BrainUint number_of_neuron = layer->_number_of_neuron;
        BrainUint i = 0;

        for (i = 0; i < number_of_neuron; ++i)
        {
            BrainNeuron neuron = layer->_neurons[i];

            if (neuron != NULL)
            {
                apply_neuron_correction(neuron);
            }
        }
    }
}

void
activate_layer(BrainLayer layer, const BrainBool hidden_layer)
{
    if (layer != NULL)
    {
        BrainUint i = 0;

        memset(layer->_in_errors, 0, layer->_number_of_neuron * sizeof(BrainDouble));

        for (i = 0; i < layer->_number_of_neuron; ++i)
        {
            BrainNeuron neuron = get_layer_neuron(layer, i);
            activate_neuron(neuron, hidden_layer);
        }
    }
}
