#include "brain_settings.h"
#include "brain_costfunction.h"
#include "brain_activation.h"
#include "brain_learning.h"

/**
 * \struct Settings
 * \brief  Internal model for a BrainSettings
 *
 * All protected fields for a BrainSettings
 */
struct Settings
{
    BrainUint         _iterations;                       /*!< maximum number of iteration in training mode  */
    BrainDouble       _error;                            /*!< target error to rich in training mode         */
    BrainDouble       _resilient_delta_max;              /*!< maximum value of delta for resilient learning */
    BrainDouble       _resilient_delta_min;              /*!< minimum value of delta for resilient learning */
    BrainDouble       _resilient_eta_positive;           /*!< positive value of eta for resilient learning  */
    BrainDouble       _resilient_eta_negative;           /*!< negative value of eta for resilient learning  */
    BrainDouble       _backpropagation_learning_rate;    /*!< learning rate for backpropagation             */
    BrainDouble       _dropout_percent;                  /*!< Dropout per cent                              */
    BrainBool         _use_dropout;                      /*!< Enable or disable dropout                     */
    CostPtrFunc       _network_cost_function;            /*!< CostFunction to use                           */
    CostPtrFunc       _network_cost_function_derivative; /*!< CostFunction derivative to use                */
    ActivationPtrFunc _neuron_activation;                /*!< Activation functon of the neuron              */
    ActivationPtrFunc _neuron_derivative;                /*!< Activation derivative function of the neuron  */
    LearningPtrFunc   _learning_function;                /*!< Learning function                             */
} Settings;

static BrainSettings _settings = NULL;

BrainSettings
get_settings_instance()
{
    if (_settings == NULL)
    {
        _settings = (BrainSettings)calloc(1, sizeof(Settings));

        _settings->_iterations                       = 1000;
        _settings->_error                            = 0.001;
        _settings->_use_dropout                      = BRAIN_FALSE;
        _settings->_dropout_percent                  = 0.5;
        _settings->_resilient_delta_max              = 0.000001;
        _settings->_resilient_delta_min              = 50.0;
        _settings->_resilient_eta_negative           = 0.95;
        _settings->_resilient_eta_positive           = 1.2;
        _settings->_backpropagation_learning_rate    = 1.12;
        _settings->_learning_function                = get_learning_function(BackPropagation);
        _settings->_neuron_activation                = activation(Sigmoid);
        _settings->_neuron_derivative                = derivative(Sigmoid);
        _settings->_network_cost_function            = get_cost_function(CrossEntropy);
        _settings->_network_cost_function_derivative = get_cost_function_derivative(CrossEntropy);
    }

    return _settings;
}

BrainUint
get_settings_max_iterations()
{
    if (_settings != NULL)
        return _settings->_iterations;

    return 0;
}

BrainDouble
get_settings_target_error()
{
    if (_settings != NULL)
        return _settings->_error;

    return 1.0;
}

BrainDouble
get_settings_backpropagation_learning_rate()
{
    if (_settings != NULL)
        return _settings->_backpropagation_learning_rate;

    return 1.0;
}

CostPtrFunc
get_settings_network_cost_function()
{
    if (_settings != NULL)
        return _settings->_network_cost_function;

    return NULL;
}

CostPtrFunc
get_settings_network_cost_function_derivative()
{
    if (_settings != NULL)
        return _settings->_network_cost_function_derivative;

    return NULL;
}

ActivationPtrFunc
get_settings_neuron_activation()
{
    if (_settings != NULL)
        return _settings->_neuron_activation;

    return NULL;
}

ActivationPtrFunc
get_settings_neuron_derivative()
{
    if (_settings != NULL)
        return _settings->_neuron_derivative;

    return NULL;
}

LearningPtrFunc
get_settings_learning_function()
{
    if (_settings != NULL)
        return _settings->_learning_function;

    return NULL;
}

BrainDouble
get_settings_resilient_delta_max()
{
    if (_settings != NULL)
        return _settings->_resilient_delta_max;

    return 0.0;
}

BrainDouble
get_settings_resilient_delta_min()
{
    if (_settings != NULL)
        return _settings->_resilient_delta_min;

    return 0.0;
}

BrainDouble
get_settings_resilient_eta_positive()
{
    if (_settings != NULL)
        return _settings->_resilient_eta_positive;

    return 0.0;
}

BrainDouble
get_settings_resilient_eta_negative()
{
    if (_settings != NULL)
        return _settings->_resilient_eta_negative;

    return 0.0;
}

BrainDouble
get_settings_dropout_percent()
{
    if (_settings != NULL)
        return _settings->_dropout_percent;

    //no dropout
    return 1.0;
}

BrainBool
get_settings_dropout_activated()
{
    if (_settings != NULL)
        return _settings->_use_dropout;

    return BRAIN_FALSE;
}

void
new_settings(const BrainUint             iterations,
             const BrainDouble           error,
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
    BrainSettings settings = get_settings_instance();

    settings->_iterations                       = iterations;
    settings->_error                            = error;
    settings->_learning_function                = get_learning_function(learning_type);
    settings->_use_dropout                      = use_dropout;
    settings->_dropout_percent                  = dropout_factor;
    settings->_backpropagation_learning_rate    = backpropagation_learning_rate;
    settings->_resilient_delta_max              = resilient_delta_max;
    settings->_resilient_delta_min              = resilient_delta_min;
    settings->_resilient_eta_negative           = resilient_eta_negative;
    settings->_resilient_eta_positive           = resilient_eta_positive;
    settings->_neuron_activation                = activation(activation_type);
    settings->_neuron_derivative                = derivative(activation_type);
    settings->_network_cost_function            = get_cost_function(costfunction_type);
    settings->_network_cost_function_derivative = get_cost_function_derivative(costfunction_type);
}

void
delete_settings()
{
    if (_settings != NULL)
        free(_settings);
}
