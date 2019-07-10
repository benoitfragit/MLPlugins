#include "brain_core_types.h"
#include "brain_function_utils.h"
#include "brain_math_utils.h"
#include "brain_enum_utils.h"
#include "brain_memory_utils.h"

static BrainString activation_name[] = {
    "Identity",
    "Sigmoid",
    "TanH",
    "ArcTan",
    "SoftPlus",
    "Sinus",
    "ReLu"
};

static BrainString costfunction_names[] =
{
    "Quadratic",
    "CrossEntropy"
};

static BrainActivationFunction _activation_functions[][2] = {{identity,           identity_derivative},
                                                       {sigmoid,            sigmoid_derivative},
                                                       {tangeant_hyperbolic,tangeant_hyperbolic_derivative},
                                                       {co_tangeant,        co_tangeant_derivative},
                                                       {softplus,           softplus_derivative},
                                                       {sinusoid,           sinusoid_derivative},
                                                       {relu,               relu_derivative}};

static BrainCostFunction _cost_functions[][2] = {{quadratic_cost,     quadratic_cost_derivative},
                                           {crossentropy_cost,  crossentropy_cost_derivative}};


/**
 * \enum BrainCostFunctionType
 * \brief enumeration to choose network cost function
 */
typedef enum BrainCostFunctionType
{
    Quadratic,            /*!< Quadratic cost function    */
    CrossEntropy,         /*!< CrossEntropy cost function */
    Invalid_CostFunction, /*!< invalid cost function      */

    First_CostFunction = Quadratic,
    Last_CostFunction  = Invalid_CostFunction
} BrainCostFunctionType;

/**
 * \enum BrainActivationType
 * \brief enumeration to choose neurons activation function
 */
typedef enum BrainActivationType
{
    Identity,           /*!< Identity activation */
    Sigmoid,            /*!< sigmoid activation  */
    TanH,               /*!< tanh activation     */
    ArcTan,             /*!< arctan activation   */
    SoftPlus,           /*!< softplus activation */
    Sinusoid,           /*!< sinusoid activation */
    ReLu,               /*!< relu activation     */
    Invalid_Activation, /*!< Invalid activation  */

    First_Activation = Identity,
    Last_Activation = Invalid_Activation
} BrainActivationType;

/**
 * \enum BrainFunctionEnum
 * \brief Define if the function is a derivative or not
 */
typedef enum BrainFunctionType
{
    Function = 0,
    Derivative = 1
} BrainFunctionType;

BrainActivationFunction
brain_activation_function(BrainString name)
{
    BrainActivationFunction function = NULL;
    BrainActivationType activation = Sigmoid;

    if (BRAIN_ALLOCATED(name))
    {
        activation  = get_enum_values(activation_name, First_Activation, Last_Activation, name);
        function    = _activation_functions[activation][Function];
    }

    return function;
}

BrainActivationFunction
brain_derivative_function(BrainString name)
{
    BrainActivationFunction function = NULL;
    BrainActivationType activation = Sigmoid;

    if (BRAIN_ALLOCATED(name))
    {
        activation  = get_enum_values(activation_name, First_Activation, Last_Activation, name);
        function    = _activation_functions[activation][Derivative];
    }

    return function;
}

BrainCostFunction
brain_cost_function(BrainString name)
{
    BrainCostFunction function = NULL;
    BrainCostFunctionType cost = Quadratic;

    if (BRAIN_ALLOCATED(name))
    {
        cost     = get_enum_values(costfunction_names, First_CostFunction, Last_CostFunction, name);
        function = _cost_functions[cost][Function];
    }

    return function;
}

BrainCostFunction
brain_derivative_cost_function(BrainString name)
{
    BrainCostFunction function = NULL;
    BrainCostFunctionType cost = Quadratic;

    if (BRAIN_ALLOCATED(name))
    {
        cost     = get_enum_values(costfunction_names, First_CostFunction, Last_CostFunction, name);
        function = _cost_functions[cost][Derivative];
    }

    return function;
}
