#include "brain_plugin.h"
#include "brain_logging_utils.h"
#include "brain_memory_utils.h"
#include "brain_core_types.h"
#include "trainer_config.h"
#include <gtk/gtk.h>

typedef cairo_surface_t* Surface;

typedef struct BrainTrainer
{
    BrainPlugin     _plugin;
    BrainNetwork    _network;
    BrainData       _data;
    GtkWidget*      _mainWindow;
    GtkWidget*      _activation;
    GtkWidget*      _evaluation;
    GtkWidget*      _error;
    GtkWidget*      _progress;
    Surface         _surface;
    BrainUint       _source_id;
} BrainTrainer;

typedef struct CmdLineOption
{
    BrainString _short_name;
    BrainString _full_name;
    BrainString _description;
    BrainBool   _valid;
    BrainChar   _args[300];
} CmdLineOption;

static CmdLineOption _cmd_line_options[] =
{
    {"-p","--plugin=",  "Load a plugin",            BRAIN_FALSE, ""},
    {"-n","--network=", "Load a network",           BRAIN_FALSE, ""},
    {"-s","--settings=","Load settings",            BRAIN_FALSE, ""},
    {"-d","--data=",    "Load data",                BRAIN_FALSE, ""},
    {"-r","--record=",  "Set the recording path",   BRAIN_FALSE, ""},
    {"-v","--view=",    "Display the GUI",          BRAIN_FALSE, ""},
    {"-h","--help",     "Display help message",     BRAIN_TRUE,  ""}
};

static BrainTrainer _trainer = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0};

typedef enum BrainTrainerArgs
{
    PluginName,
    NetworkPath,
    SettingsPath,
    DataPath,
    RecordPath,
    Display,
    Help,

    First_Args = PluginName,
    Last_Args  = Help
} BrainTrainerArgs;

static void
help()
{
    BrainUint i = 0;
    for (i = First_Args; i <= Last_Args; ++i)
    {
        printf("[%s, %s] %s\n",_cmd_line_options[i]._short_name, _cmd_line_options[i]._full_name, _cmd_line_options[i]._description);
    }
}

static BrainBool
parse_cmd_line_options(const BrainInt argc, BrainChar** argv)
{
    BrainInt i;
    BrainUint j;
    BrainBool ret = BRAIN_TRUE;
    /**************************************************************/
    /**                     PARSING CMD LINE ARGS                **/
    /**************************************************************/
    for (i = 0; i < argc; ++i)
    {
        for (j = First_Args; j <= Last_Args; ++j)
        {
            if (!strcmp(argv[i], _cmd_line_options[j]._short_name))
            {
                _cmd_line_options[j]._valid = BRAIN_TRUE;

                if ((j != Help) &&
                    (j != Display))
                {
                    strcpy(_cmd_line_options[j]._args, argv[i + 1]);
                }
                else
                {
                    if (j == Help)
                    {
                        help();
                        ret = BRAIN_FALSE;
                    }
                }
                break;
            }
            else
            {
                if (strstr(argv[i], _cmd_line_options[j]._full_name) != NULL)
                {
                    _cmd_line_options[j]._valid = BRAIN_TRUE;

                    if ((j != Help) && (j != Display))
                    {
                        BrainChar* buffer = NULL;

                        buffer = strtok(argv[i], "=");
                        buffer = strtok(NULL, "=");

                        if (BRAIN_ALLOCATED(buffer))
                        {
                            strcpy(_cmd_line_options[j]._args, buffer);
                        }
                    }
                    else
                    {
                        if (j == Help)
                        {
                            help();
                            ret = BRAIN_FALSE;
                        }
                    }
                    break;
                }
            }
        }

        if (ret == BRAIN_FALSE)
        {
            break;
        }
    }

    return ret;
}

static void
clear_surface()
{
    cairo_t *cr;
    cr = cairo_create(_trainer._surface);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);
}

static BrainInt
apply_network_training()
{
    const BrainUint number_of_evaluating_signals = get_test_length(_trainer._plugin, _trainer._data);
    const BrainUint width  = (BrainUint)((BrainReal)gtk_widget_get_allocated_width(_trainer._evaluation) / (BrainReal)number_of_evaluating_signals);
    const BrainUint height = gtk_widget_get_allocated_width(_trainer._evaluation);

    BrainUint i = 0;
    BrainReal error = 0.;
    BrainReal progress = 0.;
    BrainInt ret = 1;
    BrainUint posX = 0;
    BrainReal* err = NULL;
    BrainReal  err_min = 0.;
    BrainReal  err_max = 0.;
    cairo_t* cr;

    BRAIN_NEW(err, BrainReal, number_of_evaluating_signals);

    if (is_training_required(_trainer._plugin, _trainer._network))
    {
        train_network_step(_trainer._plugin, _trainer._network, _trainer._data);

        for (i = 0; i < number_of_evaluating_signals; ++i)
        {
            err[i] = compute_error(_trainer._plugin, _trainer._network, _trainer._data, i);
            err[i] /= (BrainReal)number_of_evaluating_signals;
            error += err[i];

            if (i == 0 || err[i] < err_min)
            {
                err_min = err[i];
            }
            if (i == 0 || err_max < err[i])
            {
                err_max = err[i];
            }
        }

        clear_surface();
        for (i = 0; i < number_of_evaluating_signals; ++i)
        {
            BrainReal normalized_error = err[i];
            if (err_min < err_max)
            {
                normalized_error -= err_min;
                normalized_error /= (err_max - err_min);
            }
            cr = cairo_create(_trainer._surface);
            cairo_set_source_rgb(cr, normalized_error, 1. - normalized_error, 0);
            cairo_rectangle(cr, posX, err[i] * height, width, (1.- err[i])*height);
            cairo_fill(cr);
            cairo_destroy(cr);
            posX += width;
        }
        gtk_widget_queue_draw(_trainer._evaluation);

        //error /= (BrainReal)number_of_evaluating_signals;
        progress = get_training_progress(_trainer._plugin, _trainer._network);
        set_error(_trainer._plugin, _trainer._network, error);
        /**************************************************************/
        /**                    UPDATE PROGRESS BARS                  **/
        /**************************************************************/
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(_trainer._progress), progress);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(_trainer._error), error);
    }

    BRAIN_DELETE(err);

    return ret;
}


static void
on_activation_changed(GObject *activation, GParamSpec *pspec, gpointer user_data)
{
    if (gtk_switch_get_active(GTK_SWITCH(activation)))
    {
        _trainer._source_id = g_timeout_add(5, (GSourceFunc)apply_network_training, NULL);
    }
    else
    {
        g_source_remove(_trainer._source_id);
    }
}

static BrainBool
on_draw_surface(GtkWidget* widget, cairo_t *cr, gpointer data)
{
    cairo_set_source_surface(cr, _trainer._surface, 0, 0);
    cairo_paint(cr);
    return BRAIN_FALSE;
}

static BrainBool
on_configure_surface(GtkWidget* widget, GdkEventConfigure* event, gpointer data)
{
    if (BRAIN_ALLOCATED(_trainer._surface))
    {
        cairo_surface_destroy(_trainer._surface);
    }

    _trainer._surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget),
                                                          CAIRO_CONTENT_COLOR,
                                                          gtk_widget_get_allocated_width(widget),
                                                          gtk_widget_get_allocated_height(widget));

    clear_surface();

    return BRAIN_TRUE;
}

static void
on_destroy()
{
    if (BRAIN_ALLOCATED(_trainer._surface))
    {
        cairo_surface_destroy(_trainer._surface);
    }

    delete_plugin_data(_trainer._plugin, _trainer._data);
    delete_plugin_network(_trainer._plugin, _trainer._network);

    if (gtk_switch_get_active(GTK_SWITCH(_trainer._activation)))
    {
        g_source_remove(_trainer._source_id);
    }
    gtk_main_quit();
}

static void
visual_training()
{
    GtkBuilder *builder = NULL;
    /**********************************************************/
    /**                  LOADING THE NETWORK                 **/
    /**********************************************************/
    _trainer._network = new_plugin_network(_trainer._plugin, _cmd_line_options[NetworkPath]._args);
    /*********************************************************/
    /**                      LOAD THE DATA                  **/
    /*********************************************************/
    _trainer._data    = new_plugin_data(_trainer._plugin, _cmd_line_options[DataPath]._args);

    if (BRAIN_ALLOCATED(_trainer._network)
    &&  BRAIN_ALLOCATED(_trainer._data))
    {
        /******************************************************/
        /**                   TWEAKING THE NETWORK           **/
        /******************************************************/
        if (_cmd_line_options[SettingsPath]._valid)
        {
            configure_network(_trainer._plugin, _trainer._network, _cmd_line_options[SettingsPath]._args);
        }

        builder = gtk_builder_new();
        gtk_builder_add_from_file(builder, BRAIN_TRAINER_VIEW_FILE, NULL);

        _trainer._mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
        _trainer._error      = GTK_WIDGET(gtk_builder_get_object(builder, "Error"));
        _trainer._progress   = GTK_WIDGET(gtk_builder_get_object(builder, "Iteration"));
        _trainer._activation = GTK_WIDGET(gtk_builder_get_object(builder, "activation"));
        _trainer._evaluation = GTK_WIDGET(gtk_builder_get_object(builder, "evaluation"));

        g_signal_connect(_trainer._mainWindow,
                          "destroy",
                          G_CALLBACK(on_destroy),
                          NULL);

        g_signal_connect(GTK_SWITCH(_trainer._activation),
                         "notify::active",
                         G_CALLBACK(on_activation_changed),
                         NULL);

        g_signal_connect(_trainer._evaluation,
                         "draw",
                         G_CALLBACK(on_draw_surface),
                         NULL);

        g_signal_connect(_trainer._evaluation,
                         "configure-event",
                         G_CALLBACK(on_configure_surface),
                         NULL);

        gtk_builder_connect_signals(builder, NULL);
        g_object_unref(builder);

        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(_trainer._error), 1);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(_trainer._progress), 0);

        gtk_widget_show_all(_trainer._mainWindow);
        gtk_main();
    }
}

/**********************************************************************/
/**                    COMMAND LINE TRAINING                         **/
/**********************************************************************/
void
cmd_line_training()
{
    /**********************************************************/
    /**                      LOADING THE NETWORK             **/
    /**********************************************************/
    if (_cmd_line_options[NetworkPath]._valid)
    {
        _trainer._network = new_plugin_network(_trainer._plugin, _cmd_line_options[NetworkPath]._args);

        if (BRAIN_ALLOCATED(_trainer._network))
        {
            /******************************************************/
            /**                   TWEAKING THE NETWORK           **/
            /******************************************************/
            if (_cmd_line_options[SettingsPath]._valid)
            {
                configure_network(_trainer._plugin, _trainer._network, _cmd_line_options[SettingsPath]._args);
            }

            if (_cmd_line_options[DataPath]._valid)
            {
                /**********************************/
                /**       TRAIN THE NETWORK      **/
                /**********************************/
                train_network_from_file(_trainer._plugin, _trainer._network, _cmd_line_options[DataPath]._args);
                /******************************************************/
                /**                     SAVE THE NETWORK             **/
                /******************************************************/
                if (_cmd_line_options[RecordPath]._valid)
                {
                    serialize_network(_trainer._plugin, _trainer._network, _cmd_line_options[RecordPath]._args);
                }
            }
            /******************************************************/
            /**                   CLEANING THE NETWORK           **/
            /******************************************************/
            delete_plugin_network(_trainer._plugin, _trainer._network);
        }
    }
}

/**********************************************************************/
/**                    DEFINING MAIN FUNCTION                        **/
/**********************************************************************/
BrainInt
main(BrainInt argc, BrainChar** argv)
{
    if (2 <= argc)
    {
        if (parse_cmd_line_options(argc, argv))
        {
            /**************************************************************/
            /**                      LOADING THE PLUGIN                  **/
            /**************************************************************/
            if (_cmd_line_options[PluginName]._valid)
            {
                _trainer._plugin = new_plugin(_cmd_line_options[PluginName]._args);

                if (BRAIN_ALLOCATED(_trainer._plugin))
                {
                    if (_cmd_line_options[Display]._valid)
                    {
                        gtk_init(&argc, &argv);
                        visual_training();
                    }
                    else
                    {
                        cmd_line_training();
                    }

                    /**********************************************************/
                    /**                      CLEANING THE PLUGIN             **/
                    /**********************************************************/
                    delete_plugin(_trainer._plugin);
                }
            }
        }
    }
    return EXIT_SUCCESS;
}
