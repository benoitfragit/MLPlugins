#ifndef BRAIN_PLUGIN_TYPES_H
#define BRAIN_PLUGIN_TYPES_H

#include "brain_core_types.h"

typedef struct _BrainPlugin                 BrainPlugin;

typedef void         (*BrainPluginActivatePtrFunc)  (void);
typedef void         (*BrainPluginDeactivatePtrFunc)(void);
typedef BrainNetwork (*BrainLoadNetworkPtrFunc)     (BrainString);
typedef void         (*BrainConfigurePtrFunc)       (BrainNetwork,       BrainString);
typedef void         (*BrainSerializePtrFunc)       (const BrainNetwork, BrainString);
typedef void         (*BrainDeserializePtrFunc)     (BrainNetwork,       BrainString);
typedef void         (*BrainTrainFromFilePtrFunc)   (BrainNetwork,       BrainString);
typedef BrainReal    (*BrainTrainStepPtrFunc)       (BrainNetwork,       const BrainData);
typedef void         (*BrainPredictPtrFunc)         (BrainNetwork,       const BrainUint, const BrainSignal);
typedef void         (*BrainDeleteNetworkPtrFunc)   (BrainNetwork);
typedef BrainBool    (*BrainTrainingRequiredPtrFunc)(const BrainNetwork);
typedef BrainData    (*BrainLoadDataPtrFunc)        (BrainString);
typedef void         (*BrainDeleteDataPtrFunc)      (BrainData);
typedef BrainReal    (*BrainGetProgressPtrFunc)     (const BrainNetwork);

#endif /* BRAIN_PLUGIN_TYPES_H */
