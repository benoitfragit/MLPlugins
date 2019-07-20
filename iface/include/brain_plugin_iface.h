/**
 * \file brain_plugin.h
 * \brief Define the API to load a networkand  from an XML file
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to create and use a plugin
 */
#ifndef BRAIN_PLUGIN_IFACE_H
#define BRAIN_PLUGIN_IFACE_H

#include <glib.h>
#include <glib-object.h>

#include "brain_plugin_types.h"

G_BEGIN_DECLS

#define TYPE_BRAIN_PLUGIN                  (brain_plugin_get_type ())
#define BRAIN_PLUGIN(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_BRAIN_PLUGIN, BrainPlugin))
#define BRAIN_PLUGIN_IFACE(obj)            (G_TYPE_CHECK_CLASS_CAST ((obj), TYPE_BRAIN_PLUGIN, BrainPluginInterface))
#define IS_BRAIN_PLUGIN(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_BRAIN_PLUGIN))
#define BRAIN_PLUGIN_GET_IFACE(obj)        (G_TYPE_INSTANCE_GET_INTERFACE ((obj), TYPE_BRAIN_PLUGIN, BrainPluginInterface))

typedef struct _BrainPluginInterface        BrainPluginInterface;

struct _BrainPluginInterface
{
    /*< private >*/
    GTypeInterface  g_iface;

    BrainPluginActivatePtrFunc   activate;
    BrainPluginDeactivatePtrFunc deactivate;
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

GType brain_plugin_get_type (void) G_GNUC_CONST;

void         brain_plugin_activate       (BrainPlugin*);
void         brain_plugin_deactivate     (BrainPlugin*);
BrainNetwork brain_plugin_load_network   (BrainPlugin*, BrainString);
void         brain_plugin_configure      (BrainPlugin*, BrainNetwork,       BrainString);
void         brain_plugin_serialize      (BrainPlugin*, const BrainNetwork, BrainString);
void         brain_plugin_deserialize    (BrainPlugin*, BrainNetwork,       BrainString);
void         brain_plugin_train_from_file(BrainPlugin*, BrainNetwork,       BrainString);
BrainReal    brain_plugin_train          (BrainPlugin*, BrainNetwork,       const BrainData);
void         brain_plugin_predict        (BrainPlugin*, BrainNetwork,       const BrainUint, const BrainSignal);
void         brain_plugin_delete_network (BrainPlugin*, BrainNetwork);
BrainBool    brain_plugin_is_training_required(BrainPlugin*, const BrainNetwork);
BrainData    brain_plugin_load_data      (BrainPlugin*, BrainString);
void         brain_plugin_delete_data    (BrainPlugin*, BrainData);
BrainReal    brain_plugin_get_progress   (BrainPlugin*, BrainNetwork);

G_END_DECLS

#endif /* BRAIN_PLUGIN_IFACE_H */
