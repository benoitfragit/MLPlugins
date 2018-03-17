#ifndef BRAIN_TRAINER_H
#define BRAIN_TRAINER_H

#include <glib.h>
#include <glib-object.h>

#include "brain_trainer_types.h"

G_BEGIN_DECLS

#define TYPE_BRAIN_TRAINER                  (brain_trainer_get_type ())
#define BRAIN_TRAINER(obj)                   (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_BRAIN_TRAINER, BrainTrainer))
#define IS_BRAIN_TRAINER(obj)                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_BRAIN_TRAINER))
#define BRAIN_TRAINER_CLASS(klass)           (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_BRAIN_TRAINER, BrainTrainerClass))
#define IS_BRAIN_TRAINER_CLASS(klass)        (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_BRAIN_TRAINER))
#define BRAIN_TRAINER_GET_CLASS(obj)         (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_BRAIN_TRAINER, BrainTrainerClass))

typedef struct _BrainTrainer                 BrainTrainer;
typedef struct _BrainTrainerPrivate          BrainTrainerPrivate;
typedef struct _BrainTrainerClass            BrainTrainerClass;

struct _BrainTrainer
{
  /*< private >*/
  GInitiallyUnowned parent_instance;

  BrainTrainerPrivate* priv;
};

struct _BrainTrainerClass
{
  /*< private >*/
  GInitiallyUnownedClass parent_class;

  /*public funcs and data here*/
};

GType brain_trainer_get_type (void) G_GNUC_CONST;

BrainTrainer*   brain_trainer_new(BrainInt argc, BrainChar** argv);
void            brain_trainer_restart(BrainTrainer* trainer);
BrainInt        brain_trainer_run(BrainTrainer* trainer);

G_END_DECLS

#endif /* BRAIN_TRAINER_H */
