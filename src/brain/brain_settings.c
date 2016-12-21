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
    BrainDouble     _resilient_delta_max;              /*!< maximum value of delta for resilient learning */
    BrainDouble     _resilient_delta_min;              /*!< minimum value of delta for resilient learning */
    BrainDouble     _resilient_eta_positive;           /*!< positive value of eta for resilient learning  */
    BrainDouble     _resilient_eta_negative;           /*!< negative value of eta for resilient learning  */
    BrainDouble     _backpropagation_learning_rate;    /*!< learning rate for backpropagation             */
    BrainDouble     _dropout_percent;                  /*!< Dropout per cent                              */
    BrainBool       _use_dropout;                      /*!< Enable or disable dropout                     */
    CostPtrFunc     _network_cost_function;            /*!< CostFunction to use                           */
    CostPtrFunc     _network_cost_function_derivative; /*!< CostFunction derivative to use                */
    PtrFunc         _neuron_activation;                /*!< Activation functon of the neuron              */
    PtrFunc         _neuron_derivative;                /*!< Activation derivative function of the neuron  */
    LearningPtrFunc _learning_function;                /*!< Learning function                             */
} Settings;

BrainDouble
get_settings_backpropagation_learning_rate(const BrainSettings settings)
{
    if (settings != NULL)
        return settings->_backpropagation_learning_rate;

    return 1.0;
}

CostPtrFunc
get_settings_network_cost_function(const BrainSettings settings)
{
    if (settings != NULL)
        return settings->_network_cost_function;

    return NULL;
}

CostPtrFunc
get_settings_network_cost_function_derivative(const BrainSettings settings)
{
    if (settings != NULL)
        return settings->_network_cost_function_derivative;

    return NULL;
}

PtrFunc
get_settings_neuron_activation(const BrainSettings settings)
{
    if (settings != NULL)
        return settings->_neuron_activation;

    return NULL;
}

PtrFunc
get_settings_neuron_derivative(const BrainSettings settings)
{
    if (settings != NULL)
        return settings->_neuron_derivative;

    return NULL;
}

LearningPtrFunc
get_settings_learning_function(const BrainSettings settings)
{
    if (settings != NULL)
        return settings->_learning_function;

    return NULL;
}

BrainDouble
get_settings_resilient_delta_max(const BrainSettings settings)
{
    if (settings != NULL)
        return settings->_resilient_delta_max;

    return 0.0;
}

BrainDouble
get_settings_resilient_delta_min(const BrainSettings settings)
{
    if (settings != NULL)
        return settings->_resilient_delta_min;

    return 0.0;
}

BrainDouble
get_settings_resilient_eta_positive(const BrainSettings settings)
{
    if (settings != NULL)
        return settings->_resilient_eta_positive;

    return 0.0;
}

BrainDouble
get_settings_resilient_eta_negative(const BrainSettings settings)
{
    if (settings != NULL)
        return settings->_resilient_eta_negative;

    return 0.0;
}

BrainDouble
get_settings_dropout_percent(const BrainSettings settings)
{
    if (settings != NULL)
        return settings->_dropout_percent;

    //no dropout
    return 1.0;
}

BrainBool
get_settings_dropout_activated(const BrainSettings settings)
{
    if (settings != NULL)
        return settings->_use_dropout;

    return BRAIN_FALSE;
}

BrainSettings
new_settings(const BrainActivationType   activation_type,
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
    BrainSettings _settings   = (BrainSettings)calloc(1, sizeof(Settings));

    _settings->_learning_function                = get_learning_function(learning_type);
    _settings->_use_dropout                      = use_dropout;
    _settings->_dropout_percent                  = dropout_factor;
    _settings->_backpropagation_learning_rate    = backpropagation_learning_rate;
    _settings->_resilient_delta_max              = resilient_delta_max;
    _settings->_resilient_delta_min              = resilient_delta_min;
    _settings->_resilient_eta_negative           = resilient_eta_negative;
    _settings->_resilient_eta_positive           = resilient_eta_positive;
    _settings->_neuron_activation                = activation(activation_type);
    _settings->_neuron_derivative                = derivative(activation_type);
    _settings->_network_cost_function            = get_cost_function(costfunction_type);
    _settings->_network_cost_function_derivative = get_cost_function_derivative(costfunction_type);

    return _settings;
}

void
delete_settings(BrainSettings settings)
{
    if (settings != NULL)
        free(settings);
}
