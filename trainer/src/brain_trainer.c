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
    GtkWidget*      _activation;
    GtkWidget*      _evaluation;
    GtkWidget*      _error;
    GtkWidget*      _progress;
    GtkWidget*      _restart;
    Surface         _surface;
    BrainUint       _source_id;
} BrainTrainer;

static BrainChar* _plugin_name;
static BrainChar* _network_path;
static BrainChar* _settings_path;
static BrainChar* _data_path;
static BrainChar* _record_path;

static BrainTrainer _trainer = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,0};

static const GOptionEntry entries[] =
{
    {"plugin",  'p', 0, G_OPTION_ARG_STRING,    &(_plugin_name),  "Plugin name use to load the Network",  NULL},
    {"network", 'n', 0, G_OPTION_ARG_FILENAME,  &(_network_path), "Network definition File to be loaded", NULL},
    {"settings",'s', 0, G_OPTION_ARG_FILENAME,  &(_settings_path),"File to load network settings",       NULL},
    {"data",    'd', 0, G_OPTION_ARG_FILENAME,  &(_data_path),    "File to load training data",           NULL},
    {"record",  'r', 0, G_OPTION_ARG_FILENAME,  &(_record_path),  "File to save the Network",             NULL},
    {NULL}
};

static void
on_open_network(GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
}

static void
on_open_settings(GSimpleAction *action,
                 GVariant      *parameter,
                 gpointer       user_data)
{
}

static void
on_open_data(GSimpleAction *action,
             GVariant      *parameter,
             gpointer       user_data)
{
}

static const GActionEntry actions[] =
{
    {"open_network",  on_open_network},
    {"open_settings", on_open_settings},
    {"open_data",     on_open_data}
};

static BrainBool
parse_cmd_line_options(BrainInt argc, BrainChar** argv)
{
    GError* error = NULL;
    GOptionContext* context = NULL;
    BrainBool ret = BRAIN_TRUE;

    context = g_option_context_new("- Loading all parameters to train a network");

    //g_option_context_set_ignore_unknown_options(context, TRUE);
    g_option_context_add_main_entries(context, entries, NULL);
    //g_option_context_add_group(context, gtk_get_action_group(TRUE));

    if (!g_option_context_parse(context, &argc, &argv, &error))
    {
        BRAIN_CRITICAL("Unable to parse commande line options");
        ret = BRAIN_FALSE;
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
            cairo_rectangle(cr, posX, (1. - normalized_error) * height, width, normalized_error*height);
            cairo_fill(cr);
            cairo_destroy(cr);
            posX += width;
        }
        gtk_widget_queue_draw(_trainer._evaluation);

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
    gtk_widget_set_sensitive(GTK_WIDGET(_trainer._restart), BRAIN_TRUE);

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
on_destroy(GtkWindow* window, gpointer user_data)
{
    Surface surface = (Surface)user_data;
    if (BRAIN_ALLOCATED(surface))
    {
        cairo_surface_destroy(surface);
    }
}

static void
on_restart()
{
    const BrainBool previous_activation = gtk_switch_get_active(GTK_SWITCH(_trainer._activation));

    if (BRAIN_ALLOCATED(_trainer._network))
    {
        delete_plugin_network(_trainer._plugin, _trainer._network);
    }

    if (previous_activation)
    {
        clear_surface();
        g_source_remove(_trainer._source_id);
    }

    gtk_switch_set_active(GTK_SWITCH(_trainer._activation), BRAIN_FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(_trainer._restart), BRAIN_FALSE);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(_trainer._error), 1);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(_trainer._progress), 0);

    _trainer._network = new_plugin_network(_trainer._plugin, _network_path);
    configure_network(_trainer._plugin, _trainer._network, _settings_path);
}

static void
on_activation(GtkApplication* app, gpointer user_data)
{
    if (BRAIN_ALLOCATED(app))
    {
        GtkBuilder *builder = NULL;
        GtkWidget  *window = NULL;
        GtkWidget  *headerbar = NULL;
        GMenu* menu = NULL;
        /**********************************************************/
        /**                   CREATE A PLUGIN                    **/
        /**********************************************************/
        _trainer._plugin = new_plugin(_plugin_name);
        /**********************************************************/
        /**                  LOADING THE NETWORK                 **/
        /**********************************************************/
        _trainer._network = new_plugin_network(_trainer._plugin,  _network_path);
        /*********************************************************/
        /**                      LOAD THE DATA                  **/
        /*********************************************************/
        _trainer._data    = new_plugin_data(_trainer._plugin, _data_path);

        if (BRAIN_ALLOCATED(_trainer._network)
        &&  BRAIN_ALLOCATED(_trainer._data))
        {
            /******************************************************/
            /**                   TWEAKING THE NETWORK           **/
            /******************************************************/
            configure_network(_trainer._plugin, _trainer._network, _settings_path);

            builder = gtk_builder_new();
            gtk_builder_add_from_file(builder, BRAIN_TRAINER_VIEW_FILE, NULL);

            window               = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
            headerbar            = GTK_WIDGET(gtk_builder_get_object(builder, "headerbar"));
            _trainer._error      = GTK_WIDGET(gtk_builder_get_object(builder, "Error"));
            _trainer._progress   = GTK_WIDGET(gtk_builder_get_object(builder, "Iteration"));
            _trainer._activation = GTK_WIDGET(gtk_builder_get_object(builder, "activation"));
            _trainer._evaluation = GTK_WIDGET(gtk_builder_get_object(builder, "evaluation"));
            _trainer._restart    = GTK_WIDGET(gtk_builder_get_object(builder, "restart"));

            menu = g_menu_new();
            g_menu_append(menu, "Open network",  "app.open_network");
            g_menu_append(menu, "Load settings", "app.open_settings");
            g_menu_append(menu, "Open data",     "app.open_data");

            g_action_map_add_action_entries (G_ACTION_MAP (app), actions, G_N_ELEMENTS (actions), app);
            gtk_application_set_app_menu (GTK_APPLICATION (app), G_MENU_MODEL (menu));
            g_object_unref (menu);

            g_signal_connect(window,
                              "destroy",
                              G_CALLBACK(on_destroy),
                              _trainer._surface);
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
            g_signal_connect(_trainer._restart,
                             "clicked",
                             G_CALLBACK(on_restart),
                             NULL);

            gtk_builder_connect_signals(builder, NULL);
            g_object_unref(builder);

            gtk_application_window_set_show_menubar(GTK_WINDOW(window), FALSE);
            g_object_set (gtk_widget_get_settings(headerbar),
                          "gtk-shell-shows-app-menu", FALSE,
                          "gtk-decoration-layout", "menu:close",
                          NULL);
            gtk_application_add_window(GTK_APPLICATION(app), GTK_WINDOW(window));
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(_trainer._error), 1);
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(_trainer._progress), 0);
            gtk_widget_set_sensitive(_trainer._restart, BRAIN_FALSE);

            gtk_widget_show_all(window);
        }
    }
}

/**********************************************************************/
/**                    COMMAND LINE TRAINING                         **/
/**********************************************************************/
static void
cmd_line_training()
{
    /**********************************************************/
    /**                      LOADING THE NETWORK             **/
    /**********************************************************/
    _trainer._network = new_plugin_network(_trainer._plugin, _network_path);

    if (BRAIN_ALLOCATED(_trainer._network))
    {
        /******************************************************/
        /**                   TWEAKING THE NETWORK           **/
        /******************************************************/
        configure_network(_trainer._plugin, _trainer._network, _settings_path);
        /**********************************/
        /**       TRAIN THE NETWORK      **/
        /**********************************/
        train_network_from_file(_trainer._plugin, _trainer._network, _data_path);
        /******************************************************/
        /**                     SAVE THE NETWORK             **/
        /******************************************************/
        serialize_network(_trainer._plugin, _trainer._network, _record_path);
        /******************************************************/
        /**                   CLEANING THE NETWORK           **/
        /******************************************************/
        delete_plugin_network(_trainer._plugin, _trainer._network);
    }
}

/**********************************************************************/
/**                    DEFINING MAIN FUNCTION                        **/
/**********************************************************************/
BrainInt
main(BrainInt argc, BrainChar** argv)
{
    GtkApplication *app = NULL;
    BrainInt status;
    /******************************************************************/
    /**                        PARSE OPTIONS                         **/
    /******************************************************************/
    parse_cmd_line_options(argc, argv);
    /******************************************************************/
    /**                        CREATING APP                          **/
    /******************************************************************/
    app = gtk_application_new("org.brain.trainer", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activation), NULL);
    /******************************************************************/
    /**                        RUNNING APP                           **/
    /******************************************************************/
    status = g_application_run(G_APPLICATION(app), argc, argv);
    /******************************************************************/
    /**                        CLEANING EVERYTHING                   **/
    /******************************************************************/
    g_object_unref(app);
    delete_plugin_data(_trainer._plugin, _trainer._data);
    delete_plugin_network(_trainer._plugin, _trainer._network);
    delete_plugin(_trainer._plugin);
    BRAIN_DELETE(_plugin_name);
    BRAIN_DELETE(_network_path);
    BRAIN_DELETE(_settings_path);
    BRAIN_DELETE(_data_path);
    BRAIN_DELETE(_record_path);

    return status;
}
