#ifndef BRAIN_DATA_MANAGER_H
#define BRAIN_DATA_MANAGER_H

#include <glib.h>
#include <gio/gio.h>
#include <glib-object.h>

#include "brain_trainer_types.h"

G_BEGIN_DECLS

#define TYPE_BRAIN_DATA_MANAGER                  (brain_data_manager_get_type ())
#define BRAIN_DATA_MANAGER(obj)                   (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_BRAIN_DATA_MANAGER, BrainDataManager))
#define IS_BRAIN_DATA_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_BRAIN_DATA_MANAGER))
#define BRAIN_DATA_MANAGER_CLASS(klass)           (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_BRAIN_DATA_MANAGER, BrainDataManagerClass))
#define IS_BRAIN_DATA_MANAGER_CLASS(klass)        (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_BRAIN_DATA_MANAGER))
#define BRAIN_DATA_MANAGER_GET_CLASS(obj)         (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_BRAIN_DATA_MANAGER, BrainDataManagerClass))

typedef struct _BrainDataManager                 BrainDataManager;
typedef struct _BrainDataManagerPrivate          BrainDataManagerPrivate;
typedef struct _BrainDataManagerClass            BrainDataManagerClass;

struct _BrainDataManager
{
  /*< private >*/
  GInitiallyUnowned parent_instance;

  BrainDataManagerPrivate* priv;
};

struct _BrainDataManagerClass
{
  /*< private >*/
  GInitiallyUnownedClass parent_class;

  /*public funcs and data here*/
};

GType brain_data_manager_get_type (void) G_GNUC_CONST;

BrainDataManager* brain_data_manager_new                ();
void              brain_data_manager_from_variant       (BrainDataManager* manager, GVariant* list);
BrainData         brain_data_manager_get_activated_data (BrainDataManager* manager);
void              brain_data_manager_add_new_data       (BrainDataManager* manager, gchar* name, BrainDataParameters parameters);
void              brain_data_manager_deactivate_data    (BrainDataManager* manager);
GVariant*         brain_data_manager_to_variant         (BrainDataManager* manager);


G_END_DECLS

#endif /* BRAIN_DATA_MANAGER_H */
