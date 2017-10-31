/** \internal
 * \file brain_builder_types.h
 * \brief Define all BrainBuilder types
 * \author Benoit F.
 * \date 11 decembre 2016
 *
 * This file contains all predefines types used in this library
 */
#include <stdio.h>
#include <stdlib.h>

#include "brain_core_types.h"

#ifndef BRAIN_BUILDER_TYPES_H
#define BRAIN_BUILDER_TYPES_H

typedef BrainNetwork (*BrainLoaderPtrFunc)     (BrainString);
typedef void         (*BrainConfigurePtrFunc)  (BrainNetwork,       BrainString);
typedef void         (*BrainSerializePtrFunc)  (const BrainNetwork, BrainString);
typedef void         (*BrainDeserializePtrFunc)(BrainNetwork,       BrainString);
typedef void         (*BrainTrainPtrFunc)      (BrainNetwork,       BrainString,     BrainString);
typedef void         (*BrainPredictPtrFunc)    (BrainNetwork,       const BrainUint, const BrainSignal);
typedef void         (*BrainDeletePtrFunc)     (BrainNetwork);

typedef struct Builder* BrainBuilder;

#endif /* BRAIN_BUILDER_TYPES_H */
