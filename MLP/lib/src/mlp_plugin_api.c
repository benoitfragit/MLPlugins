#include "mlp_api.h"

#include "mlp_trainer.h"
#include "mlp_network.h"
#include "mlp_layer.h"

#include "brain_data_utils.h"
#include "brain_memory_utils.h"
#include "brain_logging_utils.h"

#include "mlp_config.h"

#ifdef _GNUC
#define __MLP_VISIBLE__ __attribute__((visibility("default")))
#else
#define __MLP_VISIBLE__
#endif 

void __MLP_VISIBLE__
mlp_plugin_init()
{
    brain_logging_init();
}

MLPMetaData __MLP_VISIBLE__
mlp_plugin_metadata()
{
    return &mlpInfos;
}

