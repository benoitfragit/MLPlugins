#include "brain_neuron_builder.h"

double
get_weighted_delta(Neuron *neuron, const int index)
{
    if (neuron && 0 <= index && index < neuron->_number_of_input)
    {
        return neuron->_delta * neuron->_w[index];
    }

    return 0.0;
}

void
append_delta(Neuron* neuron, const double delta)
{
    if (neuron != NULL)
    {
        neuron->_delta += neuron->_out * (1.0 - neuron->_out) * delta;
    }
}

void
propagate(Neuron* neuron, const double out, const int input_index)
{
    if (neuron && 0 <= input_index && input_index < neuron->_number_of_input)
    {
        neuron->_in[input_index] = out;
    }
}

void
set_neuron_input(Neuron* neuron, const int number_of_inputs, const double* in)
{
    int k = 0;

    if (neuron != NULL && neuron->_number_of_input == number_of_inputs)
    {
        for (k = 0; k < neuron->_number_of_input; ++k)
        {
            neuron->_in[k] = in[k];
        }

        activate(neuron);
    }
}

double
weight(Neuron* neuron, const int weight_index)
{
    if (neuron != NULL
    && 0 <= weight_index
    && weight_index < neuron->_number_of_input)
    {
        return neuron->_w[weight_index];
    }

    return 0.0;
}

double
input(Neuron* neuron, const int input_index)
{
    if (neuron != NULL
    && 0 <= input_index
    && input_index < neuron->_number_of_input)
    {
        return neuron->_in[input_index];
    }

    return 0.0;
}

void
activate(Neuron *neuron)
{
    int j;
    if (neuron != NULL)
    {
        double sum = 0.0;

        for (j = 0; j < neuron->_number_of_input + 1; ++j)
        {
            sum += neuron->_in[j] * neuron->_w[j];
        }

        neuron->_out = 1.0 / (1.0 + exp(-sum));
        neuron->_delta = 0.0;
    }
}

void
update(Neuron* neuron)
{
    int index = 0;

    if (neuron != NULL)
    {
        for (index = 0; index < neuron->_number_of_input; ++index)
        {
            double correction = - neuron->_learning_rate * neuron->_delta * neuron->_in[index] + neuron->_inertial_factor * neuron->_correction[index];

            neuron->_w[index] += correction;
            neuron->_correction[index] = correction;
        }
    }
}

void
delete_neuron(Neuron* neuron)
{
    if (neuron)
    {
        if (neuron->_in != NULL)
        {
            free(neuron->_in);
        }

        if (neuron->_w != NULL)
        {
            free(neuron->_w);
        }

        if (neuron->_correction != NULL)
        {
            free(neuron->_correction);
        }

        BRAIN_LOG("Neuron", "debug", "Neuron %d has been deleted", neuron->_id);

        free(neuron);

    }
}


Neuron*
new_neuron_from_context(Context context)
{
    int index = 0;
    Context weight_context;
    Neuron* _neuron = (Neuron *)malloc(sizeof(Neuron));

    srand(time(NULL));

    if (!context || !is_node_with_name(context, "neuron"))
    {
        BRAIN_LOG ("Neuron", "critical", "<%s:%d> Context is not valid !\n",  __FILE__, __LINE__);
        return NULL;
    }

    _neuron->_learning_rate   = node_get_double(context, "learning-rate", 0.0);
    _neuron->_inertial_factor = node_get_double(context, "inertial-factor", 0.0);
    _neuron->_out             = 0.0;
    _neuron->_delta           = 0.0;
    _neuron->_number_of_input = node_get_int(context, "input", 0);
    _neuron->_in              = (double *)malloc((_neuron->_number_of_input + 1) * sizeof(double));
    _neuron->_w               = (double *)malloc((_neuron->_number_of_input + 1) * sizeof(double));
    _neuron->_correction      = (double *)malloc((_neuron->_number_of_input + 1) * sizeof(double)) ;
    _neuron->_id              = node_get_int(context, "id", 0);

    memset(_neuron->_correction, 0, (_neuron->_number_of_input + 1) * sizeof(double));

    BRAIN_LOG("Neuron", "info", "id: %d, inputs = %d, learning-rate=%lf, inertie=%lf", _neuron->_id,
                                                                                       _neuron->_number_of_input,
                                                                                       _neuron->_learning_rate,
                                                                                       _neuron->_inertial_factor);

    for (index = 0; index < _neuron->_number_of_input + 1; ++index)
    {
        weight_context = get_node_with_name_and_index(context, "weight", index);

        if (weight_context)
        {
            _neuron->_w[index] = node_get_double(weight_context, "value", (double)rand() / (double)RAND_MAX);
        }
        else
        {
            _neuron->_w[index] = (double)rand() / (double)RAND_MAX;
        }

        BRAIN_LOG("Neuron", "info", "weight %d : %lf", index, _neuron->_w[index]);
    }

    return _neuron;
}
