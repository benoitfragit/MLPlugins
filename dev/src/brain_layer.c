#include "brain_layer.h"
#include "brain_neuron.h"
#include "brain_settings.h"

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
} Layer;

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

BrainDouble
backpropagate_output_layer(BrainLayer output_layer,
                           const BrainUint number_of_output,
                           const BrainSignal desired)
{
    BrainDouble error = 0.0;

    if ((output_layer != NULL)
    &&  (desired != NULL))
    {
        LearningPtrFunc learning_function = get_settings_learning_function();

        if (learning_function != NULL)
        {
            const BrainSignal output           = get_layer_output(output_layer);
            const BrainUint   number_of_neuron = get_layer_number_of_neuron(output_layer);

            if (number_of_neuron == number_of_output)
            {
                CostPtrFunc cost_function            = get_settings_network_cost_function();
                CostPtrFunc cost_function_derivative = get_settings_network_cost_function_derivative();

                BrainUint output_index = 0;

                for (output_index = 0;
                     output_index < number_of_output;
                   ++output_index)
                {
                    const BrainDouble loss = cost_function_derivative(output[output_index], desired[output_index]);

                    BrainNeuron output_neuron = get_layer_neuron(output_layer, output_index);

                    error += cost_function(output[output_index], desired[output_index]);

                    if (output_neuron != NULL)
                    {
                        learning_function(output_neuron, loss);
                    }
                }
            }
        }
    }

    error *= 1.0/(BrainDouble)number_of_output;

    return error;
}

void
backpropagate_hidden_layer(BrainLayer hidden_layer)
{
    if (hidden_layer != NULL)
    {
        LearningPtrFunc learning_function = get_settings_learning_function();

        if (learning_function != NULL)
        {
            const BrainUint current_number_of_neuron = get_layer_number_of_neuron(hidden_layer);

            BrainUint i = 0;

            for (i = 0; i < current_number_of_neuron; ++i)
            {
                BrainNeuron current_neuron = get_layer_neuron(hidden_layer, i);

                if (current_neuron != NULL)
                {
                    const BrainDouble loss = hidden_layer->_in_errors[i];

                    learning_function(current_neuron, loss);
                }
            }
        }
    }
}

void
apply_layer_correction(BrainLayer layer)
{
    if (layer != NULL)
    {
        const BrainUint number_of_neuron = get_layer_number_of_neuron(layer);
        BrainUint i = 0;

        for (i = 0; i < number_of_neuron; ++i)
        {
            BrainNeuron neuron = get_layer_neuron(layer, i);

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
