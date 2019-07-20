/**
 * \file brain_plugin.h
 * \brief Define the API to load a networkand  from an XML file
 * \author Benoit F.
 * \date 16 decembre 2016
 *
 * All public methods to create and use a plugin
 */
#ifndef BRAIN_PLUGIN_H
#define BRAIN_PLUGIN_H
#include <glib.h>
#include <glib-object.h>
#include "brain_trainer_types.h"

G_BEGIN_DECLS

#define TYPE_BRAIN_PLUGIN                  (brain_plugin_get_type ())
#define BRAIN_PLUGIN(obj)                   (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_BRAIN_PLUGIN, BrainPlugin))
#define IS_BRAIN_PLUGIN(obj)                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_BRAIN_PLUGIN))
#define BRAIN_PLUGIN_CLASS(klass)           (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_BRAIN_PLUGIN, BrainPluginClass))
#define IS_BRAIN_PLUGIN_CLASS(klass)        (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_BRAIN_PLUGIN))
#define BRAIN_PLUGIN_GET_CLASS(obj)         (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_BRAIN_PLUGIN, BrainPluginClass))

typedef struct _BrainPlugin                 BrainPlugin;
typedef struct _BrainPluginPrivate          BrainPluginPrivate;
typedef struct _BrainPluginClass            BrainPluginClass;

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

struct _BrainPlugin
{
    /*< private >*/
    GInitiallyUnowned parent_instance;

    BrainPluginPrivate* priv;

    /* public funcs and data here */
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

struct _BrainPluginClass
{
    /*< private >*/
    GInitiallyUnownedClass parent_class;
};

GType brain_plugin_get_type (void) G_GNUC_CONST;

BrainPlugin* brain_plugin_new(BrainString plugin_name);

G_END_DECLS

#endif /* BRAIN_PLUGIN_H */
