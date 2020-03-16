#include "mlp_types.h"
#include "mlp_trainer.h"
#include "mlp_network.h"
#include "mlp_layer.h"
#include "mlp_config.h"

#include "brain_data_utils.h"
#include "brain_random_utils.h"
#include "brain_function_utils.h"
#include "brain_xml_utils.h"
#include "brain_logging_utils.h"
#include "brain_probe.h"
#include "brain_math_utils.h"
#include "brain_memory_utils.h"

typedef struct Trainer
{
    MLPNetwork        _network;
    MLPData           _data;
    BrainSignal       _target;
    /*********************************************************************/
    /**                      TRAINING PARAMETERS                        **/
    /*********************************************************************/
    BrainReal         _max_error;                   /*!< Maximum error threshold        */
    BrainUint         _max_iter;                    /*!< Maximum iteration              */
    BrainUint         _minibatch_size;              /*!< Minibatch size                 */
    BrainReal         _learning_rate;               /*!< BackProp learning rate         */
    BrainReal         _momemtum;                    /*!< BackProp momentum value        */
    BrainReal         _error;                       /*!< Current training error level   */
    BrainUint         _iterations;                  /*!< Current training iterrations   */
    BrainCostFunction _cost_function;               /*!< Cost function                  */
    BrainCostFunction _cost_function_derivative;    /*!< Cost function derivative       */
} Trainer;

MLPTrainer
new_trainer(MLPNetwork network, MLPData data)
{
    MLPTrainer trainer = NULL;

    BRAIN_NEW(trainer, Trainer, 1);

    trainer->_network          = network;
    trainer->_data             = data;

    const BrainUint output_length = get_output_signal_length(data);
    BRAIN_NEW(trainer->_target, BrainReal, output_length);

    trainer->_max_iter         = 1000;
    trainer->_max_error        = 0.0001;
    trainer->_error            = trainer->_max_error + 1.;
    trainer->_iterations       = 0;
    trainer->_minibatch_size   = 32;
    trainer->_learning_rate    = 1.12;
    trainer->_momemtum         = 0.0;
    trainer->_cost_function    = brain_cost_function("Quadratic");
    trainer->_cost_function_derivative = brain_derivative_cost_function("Quadratic");

    return trainer;
}

void
delete_trainer(MLPTrainer trainer)
{
    if (BRAIN_ALLOCATED(trainer))
    {
        delete_data(trainer->_data);
        delete_network(trainer->_network);

        if (BRAIN_ALLOCATED(trainer->_target))
        {
            BRAIN_DELETE(trainer->_target);
        }
        BRAIN_DELETE(trainer);
    }
}

void
configure_trainer_with_context(MLPTrainer trainer, BrainString filepath)
{
    BRAIN_INPUT(configure_trainer_with_context)

    if (BRAIN_ALLOCATED(trainer) &&
        BRAIN_ALLOCATED(filepath) &&
        validate_with_xsd(filepath, SETTINGS_XSD_FILE))
    {
        Document settings_document = open_document(filepath);

        if (BRAIN_ALLOCATED(settings_document))
        {
            Context backpropagation_context = get_root_node(settings_document);

            if (BRAIN_ALLOCATED(backpropagation_context) &&
                is_node_with_name(backpropagation_context, "backpropagation"))
            {
                BrainChar* buffer                   = (BrainChar *)node_get_prop(backpropagation_context, "cost-function");
                trainer->_cost_function             = brain_cost_function(buffer);
                trainer->_cost_function_derivative  = brain_derivative_cost_function(buffer);
                trainer->_max_iter                  = node_get_int(backpropagation_context, "iterations", 1000);
                trainer->_max_error                 = (BrainReal)node_get_double(backpropagation_context, "error", 0.001);
                trainer->_minibatch_size            = node_get_int(backpropagation_context, "mini-batch-size", 32);
                trainer->_learning_rate             = (BrainReal)node_get_double(backpropagation_context, "learning-rate", 0.005);
                trainer->_momemtum                  = (BrainReal)node_get_double(backpropagation_context, "momentum", 0.001);
                trainer->_error                     = trainer->_max_error + 1.;
            }

            close_document(settings_document);
        }
    }

    BRAIN_OUTPUT(configure_trainer_with_context)
}

static BrainReal
compute_error(const MLPTrainer trainer, const BrainUint index)
{
    BRAIN_INPUT(compute_error);
    BrainReal error = 0.0;

    if (BRAIN_ALLOCATED(trainer)
    &&  BRAIN_ALLOCATED(trainer->_network)
    &&  BRAIN_ALLOCATED(trainer->_data)
    &&  (index < get_number_of_evaluating_sample(trainer->_data)))
    {
        MLPNetwork network                  = trainer->_network;
        MLPData  data                       = trainer->_data;

        const BrainUint     input_length    = get_input_signal_length(data);
        const BrainUint     output_length   = get_output_signal_length(data);
        const BrainSignal   input           = get_evaluating_input_signal(data, index);
        const BrainSignal   target          = get_evaluating_output_signal(data, index);

        BrainCostFunction cost_function = trainer->_cost_function;
        BrainSignal output = NULL;
        BrainUint j = 0;

        // only propagate the signal threw all layers
        predict(network, input_length, input);

        // grab the network output and compute the error
        // between the target and the real output
        output = get_network_output(network);

        for (j = 0; j < output_length; ++j)
        {
            error += cost_function(target[j], output[j]);
        }
    }

    BRAIN_OUTPUT(compute_error);

    return error;
}

static void
compute_total_error(MLPTrainer trainer)
{
    BRAIN_INPUT(compute_total_error);

    /********************************************************/
    /**       Check if we need to train this network       **/
    /********************************************************/
    if (BRAIN_ALLOCATED(trainer)
    &&  BRAIN_ALLOCATED(trainer->_network)
    &&  BRAIN_ALLOCATED(trainer->_data))
    {
        MLPData  data    = trainer->_data;

        const BrainUint number_of_evaluating_sample = get_number_of_evaluating_sample(data);
        BrainUint i = 0;

        trainer->_error = 0.;
        for (i = 0; i < number_of_evaluating_sample; ++i)
        {
            trainer->_error += compute_error(trainer, i);
        }

        trainer->_error /= (BrainReal)(number_of_evaluating_sample);
    }

    BRAIN_OUTPUT(compute_total_error);
}

BrainBool
is_training_required(MLPTrainer trainer)
{
    BRAIN_INPUT(is_training_required);

    BrainBool ret = BRAIN_FALSE;

    if (BRAIN_ALLOCATED(trainer))
    {
        ret = (trainer->_iterations < trainer->_max_iter)
          &&  (trainer->_max_error < trainer->_error);
    }

    BRAIN_OUTPUT(is_training_required);

    return ret;
}

BrainReal
get_training_progress(MLPTrainer trainer)
{
    BRAIN_INPUT(get_training_progress)

    BrainReal ret = 0.;

    if (BRAIN_ALLOCATED(trainer))
    {
        ret = (BrainReal)trainer->_iterations / (BrainReal)trainer->_max_iter;
    }

    BRAIN_OUTPUT(get_training_progress)

    return ret;
}

void
step(MLPTrainer trainer)
{
    BRAIN_INPUT(step);

    if (BRAIN_ALLOCATED(trainer)
    &&  BRAIN_ALLOCATED(trainer->_data)
    &&  BRAIN_ALLOCATED(trainer->_network))
    {
        MLPNetwork network = trainer->_network;
        MLPData  data    = trainer->_data;

        const BrainUint input_length  = get_input_signal_length(data);
        const BrainUint output_length = get_output_signal_length(data);
        const BrainUint number_of_training_sample = get_number_of_training_sample(data);
        const BrainCostFunction cost_function_derivative = trainer->_cost_function_derivative;

        BrainSignal input  = NULL;
        BrainSignal output = NULL;
        BrainSignal target = NULL;
        BrainSignal loss   = NULL;
        BrainUint   i = 0;
        BrainUint   minibatch_size = 0;

        BRAIN_NEW(loss, BrainReal, output_length);
        /******************************************************/
        /**         ACCUMULATE WITH RANDOM MINI-BATCH        **/
        /******************************************************/
        do
        {
            const BrainUint index = (BrainUint)BRAIN_RAND_RANGE(0, number_of_training_sample-1);

            input  = get_training_input_signal(data, index);
            target = get_training_output_signal(data, index);
            BRAIN_COPY(target, trainer->_target, BrainReal, output_length);

            /**************************************************/
            /**       FORWARD PROPAGATION OF THE SIGNAL      **/
            /**************************************************/
            feedforward(network, input_length, input, BRAIN_TRUE);

            /**************************************************/
            /**     BACKPROPAGATION USING THE TARGET SIGNAL  **/
            /**************************************************/
            output = get_network_output(network);

            /**************************************************************/
            /**               COMPUTE OUTPUT ERROR DERIVATIVE            **/
            /**************************************************************/
            for (i = 0; i < output_length; ++i)
            {
                loss[i] = cost_function_derivative(output[i], target[i]);
            }

            backpropagate(network, output_length, loss);

            ++minibatch_size;
        } while (minibatch_size < trainer->_minibatch_size);
        /**************************************************/
        /**             UPDATE NETWORK WEIGHTS           **/
        /**************************************************/
        update_network(network, trainer->_learning_rate / (BrainReal)trainer->_minibatch_size, trainer->_momemtum);

        /**************************************************/
        /**                 UPDATE ERROR LEVEL           **/
        /**************************************************/
        compute_total_error(trainer);

        /**************************************************/
        /**            INCREASE NUMBER OF EPOCH          **/
        /**************************************************/
        ++trainer->_iterations;

        if (BRAIN_ALLOCATED(loss))
        {
            BRAIN_DELETE(loss);
        }
    }

    BRAIN_OUTPUT(step);
}

BrainReal
get_trainer_error(MLPTrainer trainer)
{
    BrainReal error = 1.0;

    if (BRAIN_ALLOCATED(trainer))
    {
        error = trainer->_error;
    }

    return error;
}

void
save_trainer_progression(MLPTrainer trainer, BrainString path)
{
    if (BRAIN_ALLOCATED(trainer) && BRAIN_ALLOCATED(path) && BRAIN_ALLOCATED(trainer->_network))
    {
        serialize_network(trainer->_network, path);
    }
}

void
restore_trainer_progression(MLPTrainer trainer,
                            BrainString serialized_network,
                            BrainReal progress,
                            BrainReal error)
{
    if (BRAIN_ALLOCATED(trainer))
    {
        trainer->_iterations = (BrainUint)(trainer->_max_iter * progress);
        trainer->_error = error;

        if (BRAIN_ALLOCATED(trainer->_network) &&
            BRAIN_ALLOCATED(serialized_network))
        {
            deserialize_network(trainer->_network, serialized_network);
        }
    }
}

MLPNetwork
get_trainer_network(MLPTrainer trainer)
{
    MLPNetwork network = NULL;

    if (BRAIN_ALLOCATED(trainer))
    {
        network = trainer->_network;
    }

    return network;
}

BrainSignal
get_trainer_target_signal(MLPTrainer trainer)
{
    return trainer->_target;
}