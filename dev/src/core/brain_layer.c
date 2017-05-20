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
    BrainNeuron* _neurons;          /*!< An array of BrainNeuron           */
    BrainSignal  _out;              /*!< Output vector of the Layer        */
    BrainSignal  _weighted_deltas;  /*!< An array of weighted delta        */
    BrainUint    _number_of_neuron; /*!< The number of BrainNeuron         */
    BrainLayer   _next;             /*!< The next layer in the network     */
    BrainLayer   _prev;             /*!< The previous layer in the network */
} Layer;

void
set_layer_input(BrainLayer        layer,
                const BrainUint   number_of_inputs,
                const BrainSignal in)
{
    if (layer != NULL)
    {
        const BrainUint number_of_neurons  = layer->_number_of_neuron;
        BrainUint j = 0;

        for (j = 0; j < number_of_neurons; ++j)
        {
            BrainNeuron input_neuron = get_layer_neuron(layer, j);

            set_neuron_input(input_neuron,
                             number_of_inputs,
                             in,
                             (layer->_next != NULL));
        }

        if (layer->_next != NULL)
        {
            set_layer_input(layer->_next,
                            number_of_neurons,
                            layer->_out);
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

        if (layer->_weighted_deltas)
        {
            free(layer->_weighted_deltas);
        }

        free(layer);
    }
}

BrainLayer
get_layer_next_layer(const BrainLayer layer)
{
    if (layer != NULL)
        return layer->_next;

    return NULL;
}

BrainLayer
get_layer_previous_layer(const BrainLayer layer)
{
    if (layer != NULL)
        return layer->_prev;

    return NULL;
}

BrainLayer
new_layer(const BrainUint     number_of_neurons,
          const BrainUint     number_of_inputs,
          BrainLayer prev)
{
    if ((number_of_inputs  != 0)
    &&  (number_of_neurons != 0))
    {
        BrainLayer _layer = NULL;
        BrainSignal previous_weighted_delta = NULL;

        _layer                    = (BrainLayer)calloc(1, sizeof(Layer));
        _layer->_number_of_neuron = number_of_neurons;

        // setting the previous layer on the current layer
        _layer->_prev             = prev;

        // setting the next layer on the current layer
        _layer->_next             = NULL;

        // setting the next layer on the previous layer
        if (prev != NULL)
        {
            prev->_next = _layer;

            previous_weighted_delta = prev->_weighted_deltas;
        }

        if (0 != _layer->_number_of_neuron)
        {
            BrainUint index = 0;

            _layer->_neurons         = (BrainNeuron *)calloc(_layer->_number_of_neuron, sizeof(BrainNeuron));
            _layer->_out             = (BrainSignal  )calloc(_layer->_number_of_neuron, sizeof(BrainDouble));
            _layer->_weighted_deltas = (BrainSignal)calloc(_layer->_number_of_neuron, sizeof(BrainDouble));

            for (index = 0; index < _layer->_number_of_neuron; ++index)
            {
                _layer->_neurons[index] = new_neuron(number_of_inputs,
                                                     &(_layer->_out[index]),
                                                     previous_weighted_delta);
            }
        }

        return _layer;
    }

    return NULL;
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

static void
reset_layer_delta(BrainLayer layer)
{
    if ((layer != NULL)
    &&  (layer->_prev != NULL)
    &&  (layer->_prev->_weighted_deltas))
    {
        memset(layer->_prev->_weighted_deltas, 0, layer->_prev->_number_of_neuron * sizeof(BrainDouble));
    }
}

static BrainDouble
get_layer_weighted_delta(const BrainLayer layer,
                         const BrainUint input_index)
{
    if ((layer != NULL)
    &&  (layer->_weighted_deltas != NULL)
    &&  (input_index < layer->_number_of_neuron))
    {
        return layer->_weighted_deltas[input_index];
    }

    return 0.0;
}

void
backpropagate_output_layer(BrainLayer output_layer,
                           const BrainUint number_of_output,
                           const BrainSignal output,
                           const BrainSignal desired)
{
    if ((output_layer != NULL)
    &&  (output != NULL)
    &&  (desired != NULL))
    {
        LearningPtrFunc learning_function = get_settings_learning_function();

        if (learning_function != NULL)
        {
            const BrainUint   number_of_neuron  = get_layer_number_of_neuron(output_layer);

            if (number_of_neuron == number_of_output)
            {
                CostPtrFunc cost_function_derivative = get_settings_network_cost_function_derivative();

                BrainUint output_index = 0;
                BrainNeuron output_neuron = NULL;

                reset_layer_delta(output_layer);

                for (output_index = 0;
                     output_index < number_of_output;
                   ++output_index)
                {
                    const BrainDouble loss = cost_function_derivative(output[output_index], desired[output_index]);

                    output_neuron = get_layer_neuron(output_layer, output_index);

                    if (output_neuron != NULL)
                    {
                        learning_function(output_neuron, loss);
                    }
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
        LearningPtrFunc learning_function = get_settings_learning_function();

        if (learning_function != NULL)
        {
            const BrainUint current_number_of_neuron = get_layer_number_of_neuron(hidden_layer);

            BrainUint i = 0;

            reset_layer_delta(hidden_layer);

            for (i = 0; i < current_number_of_neuron; ++i)
            {
                BrainNeuron current_neuron = get_layer_neuron(hidden_layer, i);

                if (current_neuron != NULL)
                {
                    const BrainDouble loss = get_layer_weighted_delta(hidden_layer, i);

                    learning_function(current_neuron, loss);
                }
            }
        }
    }
}
