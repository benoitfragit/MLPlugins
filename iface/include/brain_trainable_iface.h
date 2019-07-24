/**
 * \file brain_trainable.h
 * \brief Define the API to load a networkand  from an XML file
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to create and use a trainable
 */
#ifndef BRAIN_TRAINABLE_IFACE_H
#define BRAIN_TRAINABLE_IFACE_H

#include <glib.h>
#include <glib-object.h>

#include "brain_iface_types.h"

G_BEGIN_DECLS

#define TYPE_BRAIN_TRAINABLE                  (brain_trainable_get_type ())
#define BRAIN_TRAINABLE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_BRAIN_TRAINABLE, BrainTrainable))
#define BRAIN_TRAINABLE_IFACE(obj)            (G_TYPE_CHECK_CLASS_CAST ((obj), TYPE_BRAIN_TRAINABLE, BrainTrainableInterface))
#define IS_BRAIN_TRAINABLE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_BRAIN_TRAINABLE))
#define BRAIN_TRAINABLE_GET_IFACE(obj)        (G_TYPE_INSTANCE_GET_INTERFACE ((obj), TYPE_BRAIN_TRAINABLE, BrainTrainableInterface))

typedef struct _BrainTrainableInterface        BrainTrainableInterface;

struct _BrainTrainableInterface
{
    /*< private >*/
    GTypeInterface  g_iface;

    BrainTrainableActivatePtrFunc   activate;
    BrainTrainableDeactivatePtrFunc deactivate;
    BrainLoadNetworkPtrFunc      load_network;
    BrainConfigurePtrFunc        configure;
    BrainSerializePtrFunc        serialize;
    BrainDeserializePtrFunc      deserialize;
    BrainTrainFromFilePtrFunc    train_from_file;
    BrainTrainStepPtrFunc        train;
    BrainPredictPtrFunc          predict;
    BrainDeleteNetworkPtrFunc    delete_network;
    BrainTrainingRequiredPtrFunc is_training_required;
    BrainLoadDataPtrFunc         load_data;
    BrainDeleteDataPtrFunc       delete_data;
    BrainGetProgressPtrFunc      get_progress;
};

GType brain_trainable_get_type (void) G_GNUC_CONST;

void         brain_trainable_activate       (BrainTrainable*);
void         brain_trainable_deactivate     (BrainTrainable*);
BrainNetwork brain_trainable_load_network   (BrainTrainable*, BrainString);
void         brain_trainable_configure      (BrainTrainable*, BrainNetwork,       BrainString);
void         brain_trainable_serialize      (BrainTrainable*, const BrainNetwork, BrainString);
void         brain_trainable_deserialize    (BrainTrainable*, BrainNetwork,       BrainString);
void         brain_trainable_train_from_file(BrainTrainable*, BrainNetwork,       BrainString);
BrainReal    brain_trainable_train          (BrainTrainable*, BrainNetwork,       const BrainData);
void         brain_trainable_predict        (BrainTrainable*, BrainNetwork,       const BrainUint, const BrainSignal);
void         brain_trainable_delete_network (BrainTrainable*, BrainNetwork);
BrainBool    brain_trainable_is_training_required(BrainTrainable*, const BrainNetwork);
BrainData    brain_trainable_load_data      (BrainTrainable*, BrainString);
void         brain_trainable_delete_data    (BrainTrainable*, BrainData);
BrainReal    brain_trainable_get_progress   (BrainTrainable*, BrainNetwork);

G_END_DECLS

#endif /* BRAIN_TRAINABLE_IFACE_H */
