#include "brain_trainer.h"
#include "brain_plugin.h"
#include "brain_logging_utils.h"
#include "brain_memory_utils.h"
#include "brain_core_types.h"
#include "trainer_config.h"
#define ACCESS_PRIVATE_MEMBERS(trainer) trainer->priv = G_TYPE_INSTANCE_GET_PRIVATE (trainer, TYPE_BRAIN_TRAINER, BrainTrainerPrivate);\
                                        BrainTrainerPrivate *priv = trainer->priv;
/**********************************************************************/
/**                       SETTINGS STATIC STORAGE                    **/
/**********************************************************************/
static BrainChar* _plugin_name;
static BrainChar* _network_path;
static BrainChar* _settings_path;
static BrainChar* _data_path;
static BrainChar* _record_path;
/**********************************************************************/
/**                           OPTION PARAMETERS                      **/
/**********************************************************************/
static const GOptionEntry entries[] =
{
    {"plugin",  'p', 0, G_OPTION_ARG_STRING,    &(_plugin_name),  "Plugin name use to load the Network",  NULL},
    {"network", 'n', 0, G_OPTION_ARG_FILENAME,  &(_network_path), "Network definition File to be loaded", NULL},
    {"settings",'s', 0, G_OPTION_ARG_FILENAME,  &(_settings_path),"File to load network settings",       NULL},
    {"data",    'd', 0, G_OPTION_ARG_FILENAME,  &(_data_path),    "File to load training data",           NULL},
    {"record",  'r', 0, G_OPTION_ARG_FILENAME,  &(_record_path),  "File to save the Network",             NULL},
    {NULL}
};
/**********************************************************************/
/**                        SIGNALS DEFINITION                        **/
/**********************************************************************/
enum
{
    TRAINER_SIGNAL_ITERATION,
    TRAINER_SIGNAL_LAST
};

static BrainUint trainer_signals[TRAINER_SIGNAL_LAST];
/**********************************************************************/
/**                    PROPERTY ENUM DEFINITION                      **/
/**********************************************************************/
enum
{
    PROP_O,
    PROP_PLUGIN_NAME,
    PROP_NETWORK_PATH,
    PROP_SETTINGS_PATH,
    PROP_DATA_PATH,
    PROP_RECORD_PATH,
    PROP_IS_RUNNING,
    PROP_PROGRESS,
    PROP_ERROR
};
/**********************************************************************/
/**                    DEFINING A PRIVATE BRAIN TRAINER              **/
/**********************************************************************/
typedef struct _BrainTrainerPrivate
{
    /******************************************************************/
    /**                         CORE STRUCTURE                       **/
    /******************************************************************/
    BrainPlugin*  _plugin;
    BrainNetwork _network;
    BrainData    _data;
    /******************************************************************/
    /**                           PARAMETERS                         **/
    /******************************************************************/
    BrainChar*   _plugin_name;
    BrainChar*   _network_path;
    BrainChar*   _settings_path;
    BrainChar*   _data_path;
    BrainChar*   _record_path;
    /******************************************************************/
    /**                             STATUS                           **/
    /******************************************************************/
    BrainBool    _is_running;
    BrainReal    _error;
    BrainReal    _progress;
} _BrainTrainerPrivate;
/**********************************************************************/
/**                  DEFINING BRAIN_TRAINER TYPE                     **/
/**********************************************************************/
G_DEFINE_TYPE (BrainTrainer, brain_trainer, G_TYPE_OBJECT);

static void
brain_trainer_dispose(GObject *object)
{
    G_OBJECT_CLASS (brain_trainer_parent_class)->dispose (object);
}

static void
brain_trainer_finalize(GObject *object)
{
    BrainTrainer *trainer = BRAIN_TRAINER(object);

    ACCESS_PRIVATE_MEMBERS(trainer)

    //unref all objects
    if (BRAIN_ALLOCATED(priv)
    &&  BRAIN_ALLOCATED(priv->_plugin))
    {

        priv->_plugin->delete_data(priv->_data);
        priv->_plugin->delete_network(priv->_network);

        g_object_unref(priv->_plugin);

        BRAIN_DELETE(priv->_plugin_name);
        BRAIN_DELETE(priv->_network_path);
        BRAIN_DELETE(priv->_settings_path);
        BRAIN_DELETE(priv->_data_path);
        BRAIN_DELETE(priv->_record_path);
    }

    //This object will be destroyed when the reference count will reach 0
    G_OBJECT_CLASS (brain_trainer_parent_class)->finalize (object);
}

static void
brain_trainer_set_property (GObject      *gobject,
                      guint         prop_id,
                      const GValue *value,
                      GParamSpec   *pspec)
{
    BrainTrainer* trainer = BRAIN_TRAINER(gobject);

    if (BRAIN_ALLOCATED(trainer))
    {
        ACCESS_PRIVATE_MEMBERS(trainer);

        if (BRAIN_ALLOCATED(priv))
        {
            switch (prop_id)
            {
                case PROP_PLUGIN_NAME:
                    priv->_plugin_name = g_strdup((gchar *)g_value_get_string(value));
                    break;
                case PROP_NETWORK_PATH:
                    priv->_network_path = g_strdup((gchar *)g_value_get_string(value));
                    break;
                case PROP_DATA_PATH:
                    priv->_data_path = g_strdup((gchar *)g_value_get_string(value));
                    break;
                case PROP_SETTINGS_PATH:
                    priv->_settings_path = g_strdup((gchar *)g_value_get_string(value));
                    break;
                case PROP_RECORD_PATH:
                    priv->_record_path = g_strdup((gchar *)g_value_get_string(value));
                    break;
                case PROP_IS_RUNNING:
                    priv->_is_running = g_value_get_boolean(value);
                    break;
                default:
                    G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
                    break;
            }
        }
    }
}

static void
brain_trainer_get_property (GObject    *gobject,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
    BrainTrainer* trainer = BRAIN_TRAINER(gobject);

    if (BRAIN_ALLOCATED(trainer))
    {
        ACCESS_PRIVATE_MEMBERS(trainer);

        if (BRAIN_ALLOCATED(priv))
        {
            switch (prop_id)
            {
                case PROP_PLUGIN_NAME:
                    g_value_set_string(value, priv->_plugin_name);
                    break;
                case PROP_NETWORK_PATH:
                    g_value_set_string(value, priv->_network_path);
                    break;
                case PROP_DATA_PATH:
                    g_value_set_string(value, priv->_data_path);
                    break;
                case PROP_SETTINGS_PATH:
                    g_value_set_string(value, priv->_settings_path);
                    break;
                case PROP_RECORD_PATH:
                    g_value_set_string(value, priv->_record_path);
                    break;
                case PROP_IS_RUNNING:
                    g_value_set_boolean(value, priv->_is_running);
                    break;
                case PROP_PROGRESS:
                    g_value_set_float(value, priv->_progress);
                    break;
                case PROP_ERROR:
                    g_value_set_float(value, priv->_error);
                    break;
                default:
                    G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
                    break;
            }
        }
    }
}

static void
brain_trainer_class_init(BrainTrainerClass *klass)
{
    g_type_class_add_private (klass, sizeof (BrainTrainerPrivate));
    /******************************************************************/
    /**             PASSING ALL REQUIRED FUNCTION POINTERS           **/
    /******************************************************************/
    GObjectClass *gklass = G_OBJECT_CLASS(klass);
    gklass->get_property = brain_trainer_get_property;
    gklass->set_property = brain_trainer_set_property;
    gklass->dispose      = brain_trainer_dispose;
    gklass->finalize     = brain_trainer_finalize;
    /******************************************************************/
    /**                  INSTALLING ALL PROPERTIES                   **/
    /******************************************************************/
    GParamSpec *pspec = g_param_spec_string ("plugin-name",
                                             "plugin-name",
                                             "plugin-name",
                                             NULL,
                                             G_PARAM_READWRITE);
    g_object_class_install_property (gklass, PROP_PLUGIN_NAME, pspec);

    pspec = g_param_spec_string("network-path",
                                "network-path",
                                "network-path",
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gklass, PROP_NETWORK_PATH, pspec);

    pspec = g_param_spec_string("data-path",
                                "data-path",
                                "data-path",
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gklass, PROP_DATA_PATH, pspec);

    pspec = g_param_spec_string("settings-path",
                                "settings-path",
                                "settings-path",
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gklass, PROP_SETTINGS_PATH, pspec);

    pspec = g_param_spec_string("record-path",
                                "record-path",
                                "record-path",
                                NULL,
                                G_PARAM_READWRITE);
    g_object_class_install_property(gklass, PROP_RECORD_PATH, pspec);

    pspec = g_param_spec_boolean("is-running",
                                 "is-running",
                                 "is-running",
                                 BRAIN_FALSE,
                                 G_PARAM_READWRITE);
    g_object_class_install_property(gklass, PROP_IS_RUNNING, pspec);

    pspec = g_param_spec_float("progress",
                                 "progress",
                                 "progrss",
                                 0.f,
                                 1.f,
                                 0.f,
                                 G_PARAM_READABLE);
    g_object_class_install_property(gklass, PROP_PROGRESS, pspec);

    pspec = g_param_spec_float("error",
                                 "error",
                                 "error",
                                 0.f,
                                 1.f,
                                 1.f,
                                 G_PARAM_READABLE);
    g_object_class_install_property(gklass, PROP_ERROR, pspec);
    /******************************************************************/
    /**                   DEFINING ALL SIGNALS                       **/
    /******************************************************************/
    trainer_signals[TRAINER_SIGNAL_ITERATION] = g_signal_new("trainer-iteration",
                                                         G_TYPE_FROM_CLASS(klass),
                                                         G_SIGNAL_RUN_LAST,
                                                         0,
                                                         NULL,
                                                         NULL,
                                                         NULL,
                                                         G_TYPE_NONE,
                                                         0);
}

static void
brain_trainer_init(BrainTrainer* trainer)
{
    if (BRAIN_ALLOCATED(trainer))
    {
        ACCESS_PRIVATE_MEMBERS(trainer)

        if (BRAIN_ALLOCATED(priv))
        {
            priv->_plugin       = NULL;
            priv->_network      = NULL;
            priv->_data         = NULL;
            priv->_plugin_name  = NULL;
            priv->_network_path = NULL;
            priv->_settings_path= NULL;
            priv->_data_path    = NULL;
            priv->_record_path  = NULL;
            priv->_is_running   = FALSE;
            priv->_error        = 1.;
            priv->_progress     = 0.;
        }
    }
}
/**********************************************************************/
/**                     TRAINER MAIN FUNCTION                        **/
/**********************************************************************/
gboolean
brain_trainer_run(BrainTrainer* trainer)
{
    gboolean ret = FALSE;

    if (BRAIN_ALLOCATED(trainer))
    {
        g_object_get(trainer, "is-running", &ret, NULL);

        if (ret)
        {
            ACCESS_PRIVATE_MEMBERS(trainer)

            if (BRAIN_ALLOCATED(priv)
            &&  BRAIN_ALLOCATED(priv->_plugin))
            {
                if (priv->_plugin->is_training_required(priv->_network))
                {
                    BRAIN_DEBUG(">>>>>>>>>>>>>>>>>>>>>>>>>>>>> STARTING");

                    priv->_error = priv->_plugin->train(priv->_network, priv->_data);
                    priv->_progress = priv->_plugin->get_progress(priv->_network);
                    g_signal_emit(trainer, trainer_signals[TRAINER_SIGNAL_ITERATION], 0);
                    BRAIN_DEBUG("Error:%f, progress:%f\n", priv->_error, priv->_progress);
                }
                else
                {
                    BRAIN_DEBUG(">>>>>>>>>>>>>>>>>>>>>>>>>>>>> STOPPING");
                    g_object_set(trainer, "is-running", FALSE, NULL);
                    ret = FALSE;
                }
            }
        }
    }

    return ret;
}
/**********************************************************************/
/**                      RESTART THE TRAINING                        **/
/**********************************************************************/
void
brain_trainer_restart(BrainTrainer* trainer)
{
    if (BRAIN_ALLOCATED(trainer))
    {
        ACCESS_PRIVATE_MEMBERS(trainer)

        if (BRAIN_ALLOCATED(priv)
        && BRAIN_ALLOCATED(priv->_plugin))
        {
            priv->_plugin->delete_network(priv->_network);
            priv->_network = priv->_plugin->load_network(priv->_network_path);
            priv->_plugin->configure(priv->_network, priv->_settings_path);
        }
    }
}
/**********************************************************************/
/**                          PARSING OPTIONS                         **/
/**********************************************************************/
static BrainBool
brain_trainer_parse_options(BrainInt argc, BrainChar** argv)
{
    GError          *error  = NULL;
    GOptionContext  *context= NULL;
    BrainBool       ret     = BRAIN_TRUE;

    context = g_option_context_new("- Loading all parameters to train a network");
    g_option_context_add_main_entries(context, entries, NULL);

    if (!g_option_context_parse(context, &argc, &argv, &error))
    {
        BRAIN_CRITICAL("Unable to parse commande line options");
        ret = BRAIN_FALSE;
    }

    return ret;
}
/**********************************************************************/
/**                  Create a new trainer                            **/
/**********************************************************************/
BrainTrainer*
brain_trainer_new(BrainInt argc, BrainChar** argv)
{
    BrainBool optionValid = brain_trainer_parse_options(argc, argv);
    /******************************************************************/
    /**                     CREATE A TRAINER                         **/
    /******************************************************************/
    BrainTrainer* trainer = (BrainTrainer *)g_object_new(TYPE_BRAIN_TRAINER, NULL);

    g_object_set(trainer,
                 "is-running",    FALSE,
                 "plugin-name",   _plugin_name,
                 "data-path",     _data_path,
                 "settings-path", _settings_path,
                 "network-path",  _network_path,
                 "record-path",   _record_path,
                 NULL);

    ACCESS_PRIVATE_MEMBERS(trainer)

    if (BRAIN_ALLOCATED(priv))
    {
        /**********************************************************/
        /**                   CREATE A PLUGIN                    **/
        /**********************************************************/
        priv->_plugin  = brain_plugin_new(priv->_plugin_name);
        /**********************************************************/
        /**                  LOADING THE NETWORK                 **/
        /**********************************************************/
        if (BRAIN_ALLOCATED(priv->_plugin))
        {
            priv->_network = priv->_plugin->load_network(priv->_network_path);
            /*********************************************************/
            /**                      LOAD THE DATA                  **/
            /*********************************************************/
            priv->_data    = priv->_plugin->load_data(priv->_data_path);

            if (BRAIN_ALLOCATED(priv->_network)
            &&  BRAIN_ALLOCATED(priv->_data))
            {
                /******************************************************/
                /**                   TWEAKING THE NETWORK           **/
                /******************************************************/
                priv->_plugin->configure(priv->_network, priv->_settings_path);
            }
        }
    }

    return trainer;
}

#if CMD_LINE_TRAINING_ENABLE
/**********************************************************************/
/**                    COMMAND LINE TRAINING                         **/
/**********************************************************************/
static void
cmd_line_training(BrainTrainer* trainer)
{
    if (BRAIN_ALLOCATED(trainer)
    {
        ACCESS_PRIVATE_MEMBERS(trainer);

        if (BRAIN_ALLOCATED(priv) && BRAIN_ALLOCATED(priv->_plugin))
        {
            /**********************************************************/
            /**                      LOADING THE NETWORK             **/
            /**********************************************************/
            priv->_network = priv->_plugin->load_network(priv->_network_path);

            if (BRAIN_ALLOCATED(priv->_network))
            {
                /******************************************************/
                /**                   TWEAKING THE NETWORK           **/
                /******************************************************/
                priv->_plugin->configure(priv->_network, priv->_settings_path);
                /**********************************/
                /**       TRAIN THE NETWORK      **/
                /**********************************/
                priv->_plugin->train_from_file(priv->_network, priv->_data_path);
                /******************************************************/
                /**                     SAVE THE NETWORK             **/
                /******************************************************/
                priv->_plugin->serialize(priv->_network, priv->_record_path);
                /******************************************************/
                /**                   CLEANING THE NETWORK           **/
                /******************************************************/
                priv->_plugin->delete_network(priv->_network);
            }
        }
    }
}
#endif /* CMD_LINE_TRAINING_ENABLE */
