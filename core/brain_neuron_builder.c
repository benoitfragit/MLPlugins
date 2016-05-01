#include "brain_neuron_builder.h"

struct Neuron
{
    double*        _in;
    double*        _w;
    double         _out;
    double         _learning_rate;
    double         _inertial_factor;
    double         _delta;
    double*        _correction;
    PtrFunc        _activation;
    PtrFunc        _derivative;
    int            _id;
    int            _number_of_input;
	ActivationType _activation_type;
} Neuron;

int
get_neuron_id(Neuron_t neuron)
{
	if (neuron)
	{
		return neuron->_id;
	}

	return -1;
}

double
output(Neuron_t neuron)
{
	if (neuron)
	{
		return neuron->_out;
	}

	return 0.0;
}

double
get_weighted_delta(Neuron_t neuron, const int index)
{
    if (neuron && 0 <= index && index < neuron->_number_of_input)
    {
        return neuron->_delta * neuron->_w[index];
    }

    return 0.0;
}

void
append_delta(Neuron_t neuron, const double delta)
{
    if (neuron != NULL)
    {
        neuron->_delta += neuron->_derivative(neuron->_out) * delta;
    }
}

void
propagate(Neuron_t neuron, const double out, const int input_index)
{
    if (neuron && 0 <= input_index && input_index < neuron->_number_of_input)
    {
        neuron->_in[input_index] = out;
    }
}

void
set_neuron_input(Neuron_t neuron, const int number_of_inputs, const double* in)
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
weight(Neuron_t neuron, const int weight_index)
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
input(Neuron_t neuron, const int input_index)
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
activate(Neuron_t neuron)
{
    int j;
    if (neuron != NULL)
    {
        double sum = 0.0;

        for (j = 0; j < neuron->_number_of_input + 1; ++j)
        {
            sum += neuron->_in[j] * neuron->_w[j];
        }

        neuron->_out = neuron->_activation(sum);
        neuron->_delta = 0.0;
    }
}

void
update(Neuron_t neuron)
{
    int index = 0;

    if (neuron != NULL)
    {
        for (index = 0; index < neuron->_number_of_input+1; ++index)
        {
            double correction = - neuron->_learning_rate * neuron->_delta * neuron->_in[index] + neuron->_inertial_factor * neuron->_correction[index];

            neuron->_w[index] += correction;
            neuron->_correction[index] = correction;
        }
    }
}

void
delete_neuron(Neuron_t neuron)
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

        BRAIN_DEBUG("Neuron %d has been deleted", neuron->_id);

        free(neuron);

    }
}


Neuron_t
new_neuron_from_context(Context context)
{
    int index = 0;
    Context weight_context;
    Neuron_t _neuron = (Neuron_t)malloc(sizeof(Neuron));

    srand(time(NULL));

    if (!context || !is_node_with_name(context, "neuron"))
    {
        BRAIN_CRITICAL ("<%s:%d> Context is not valid !\n",  __FILE__, __LINE__);
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

	_neuron->_activation_type  = get_activation_type((char *)node_get_prop(context, "activation-type"));
	_neuron->_activation      = activation(_neuron->_activation_type);
    _neuron->_derivative      = derivative(_neuron->_activation_type);

    memset(_neuron->_correction, 0, (_neuron->_number_of_input + 1) * sizeof(double));

    BRAIN_INFO("id: %d, inputs = %d, learning-rate=%lf, inertie=%lf, activation=%d", _neuron->_id,
                                                                                     _neuron->_number_of_input,
                                                                                     _neuron->_learning_rate,
                                                                                     _neuron->_inertial_factor,
																					 _neuron->_activation_type);

    _neuron->_in[_neuron->_number_of_input] = -1.0;

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

        BRAIN_INFO("weight %d : %lf", index, _neuron->_w[index]);
    }

    return _neuron;
}

void
dump_neuron(Neuron_t neuron, FILE* file)
{
	int i;
	if (neuron && file)
	{
		const char* activation_name = get_activation_name(neuron->_activation_type);

		fprintf(file, "\t\t<neuron");
		fprintf(file, " id=\"%d\"",               neuron->_id);
		fprintf(file, " input=\"%d\"",            neuron->_number_of_input);
		fprintf(file, " learning-rate=\"%lf\"",   neuron->_learning_rate);
		fprintf(file, " inertial-factor=\"%lf\"", neuron->_inertial_factor);

		if (!activation_name)
		{
			fprintf(file, " activation-type=\"%s\"", "sigmoid");
		}
		else
		{
			fprintf(file, " activation-type=\"%s\"", activation_name);
		}

		fprintf(file, ">\n");

		//don't forget the bias
		for (i = 0; i < neuron->_number_of_input + 1; ++i)
		{
			fprintf(file, "\t\t\t<weight value=\"%lf\"/>\n", neuron->_w[i]);
		}

		fprintf(file, "\t\t</neuron>\n");
	}
}
