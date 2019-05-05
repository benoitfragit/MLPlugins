#include "brain_memory_utils.h"
#include "brain_logging_utils.h"
#include "brain_data_manager.h"
#include "brain_core_types.h"
#include "trainer_config.h"
#include "brain_trainer.h"
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <glib.h>
/**********************************************************************/
/**                            ACTION CALLBACK                       **/
/**********************************************************************/
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
/**********************************************************************/
/**                           ACTION DEFINITION                      **/
/**********************************************************************/
static const GActionEntry actions[] =
{
    {"open_network",  on_open_network, NULL, NULL, NULL},
    {"open_settings", on_open_settings, NULL, NULL, NULL},
    {"open_data",     on_open_data, NULL, NULL, NULL}
};
/**********************************************************************/
/**                           CALLBACK FUNCTIONS                     **/
/**********************************************************************/
static void
do_toggle_sensitive(GtkWidget* widget, gpointer user_data)
{
    if (BRAIN_ALLOCATED(widget))
    {
        gtk_widget_set_sensitive(widget, !gtk_widget_get_sensitive(widget));
    }
}


static void
do_change_restart_sensivity(GObject *activation, GParamSpec *pspec, gpointer user_data)
{
    GtkWidget* widget = GTK_WIDGET(user_data);

    do_toggle_sensitive(widget, NULL);
}

static void
do_pause_start_training(GObject *activation, GParamSpec *pspec, gpointer user_data)
{
    BrainTrainer *trainer = (BrainTrainer*)user_data;

    if (BRAIN_ALLOCATED(trainer))
    {
        GValue value;

        if (gtk_switch_get_active(GTK_SWITCH(activation)))
        {
            g_object_set(trainer, "is-running", TRUE, NULL);
            g_timeout_add(5, (GSourceFunc)brain_trainer_run, trainer);
        }
        else
        {
            g_object_set(trainer, "is-running", FALSE, NULL);
        }
    }
}

static void
do_on_destroy(GtkWindow* window, gpointer user_data)
{
}

static void
do_resume_sensitivity(GtkWidget* restart, gpointer user_data)
{
    if (BRAIN_ALLOCATED(restart))
    {
        gtk_widget_set_sensitive(GTK_WIDGET(restart), BRAIN_FALSE);
    }
}

static void
do_resume_progress(GtkWidget* restart, gpointer user_data)
{
    GtkWidget* progress = (GtkWidget * )user_data;

    if (BRAIN_ALLOCATED(progress))
    {
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), 0);
    }
}

static void
do_toggle_activation(GtkSwitch* widget, gpointer user_data)
{
    if (BRAIN_ALLOCATED(widget))
    {
        gtk_switch_set_active(GTK_SWITCH(widget), gtk_switch_get_active(widget));
    }
}

static void
do_resume_training(BrainTrainer* trainer, gpointer user_data)
{
    if (BRAIN_ALLOCATED(trainer))
    {
        brain_trainer_restart(trainer);
    }
}

static void
do_modify_progress_on_iteration(BrainTrainer* trainer, gpointer user_data)
{
    GtkWidget* progress = (GtkWidget *)(user_data);

    if (BRAIN_ALLOCATED(trainer)
    &&  BRAIN_ALLOCATED(progress))
    {
        float value = 0.f;
        g_object_get(trainer, "progress", &value, NULL);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), value);
    }
}

static void
do_modify_error_on_iteration(BrainTrainer* trainer, gpointer user_data)
{
    GtkWidget* error = (GtkWidget *)(user_data);

    if (BRAIN_ALLOCATED(trainer)
    &&  BRAIN_ALLOCATED(error))
    {
        float value = 0.f;
        g_object_get(trainer, "error", &value, NULL);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(error), value);
    }
}

static void
on_activation(GtkApplication* app, gpointer user_data)
{
    BrainTrainer *trainer = (BrainTrainer *)user_data;

    if (BRAIN_ALLOCATED(trainer)
    &&  BRAIN_ALLOCATED(app))
    {
        GtkBuilder  *builder    = NULL;
        GtkWidget   *window     = NULL;
        GtkWidget   *headerbar  = NULL;
        GtkWidget   *error      = NULL;
        GtkWidget   *progress   = NULL;
        GtkWidget   *restart    = NULL;
        GtkWidget   *activation = NULL;
        GMenu       *menu       = NULL;
        /**************************************************************/
        /**                     LOADING THE UI FILE                  **/
        /**************************************************************/
        builder = gtk_builder_new();
        gtk_builder_add_from_file(builder, BRAIN_TRAINER_VIEW_FILE, NULL);
        /**************************************************************/
        /**                     LOADING ALL ELEMENTS                 **/
        /**************************************************************/
        window      = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
        headerbar   = GTK_WIDGET(gtk_builder_get_object(builder, "headerbar"));
        error       = GTK_WIDGET(gtk_builder_get_object(builder, "Error"));
        progress    = GTK_WIDGET(gtk_builder_get_object(builder, "Iteration"));
        activation  = GTK_WIDGET(gtk_builder_get_object(builder, "activation"));
        restart     = GTK_WIDGET(gtk_builder_get_object(builder, "restart"));
        /**************************************************************/
        /**                CREATE AND ATTACH A MENU                  **/
        /**************************************************************/
        menu = g_menu_new();
        g_menu_append(menu, "Open network",  "app.open_network");
        g_menu_append(menu, "Load settings", "app.open_settings");
        g_menu_append(menu, "Open data",     "app.open_data");
        g_action_map_add_action_entries (G_ACTION_MAP (app), actions, G_N_ELEMENTS (actions), trainer);
        gtk_application_set_app_menu (GTK_APPLICATION (app), G_MENU_MODEL (menu));
        g_object_unref (menu);
        /**************************************************************/
        /** CALLBACK: WINDOW DESTRUCTION                             **/
        /**************************************************************/
        g_signal_connect(window,
                         "destroy",
                          G_CALLBACK(do_on_destroy),
                          NULL);
        /**************************************************************/
        /** CALLBACK: ACTIVATE OR DEACTIVATE LIVE TRAINING            **/
        /**************************************************************/
        g_signal_connect(GTK_SWITCH(activation),
                         "notify::active",
                         G_CALLBACK(do_change_restart_sensivity),
                         restart);
        g_signal_connect(GTK_SWITCH(activation),
                         "notify::active",
                         G_CALLBACK(do_pause_start_training),
                         trainer);
        /**************************************************************/
        /** CALLBACK: RESTART                                        **/
        /**************************************************************/
        g_signal_connect(restart,
                         "clicked",
                         G_CALLBACK(do_toggle_sensitive),
                         NULL);
        g_signal_connect(restart,
                         "clicked",
                         G_CALLBACK(do_resume_progress),
                         progress);
        g_signal_connect(restart,
                         "clicked",
                         G_CALLBACK(do_resume_progress),
                         error);
        g_signal_connect_swapped(restart,
                                 "clicked",
                                 G_CALLBACK(do_toggle_activation),
                                 activation);
        g_signal_connect_swapped(restart,
                                 "clicked",
                                 G_CALLBACK(do_resume_training),
                                 trainer);
        /**************************************************************/
        /** CALLBACK: TRAINING                                       **/
        /**************************************************************/
        g_signal_connect(trainer,
                         "trainer-iteration",
                         G_CALLBACK(do_modify_progress_on_iteration),
                         progress);
        g_signal_connect(trainer,
                         "trainer-iteration",
                         G_CALLBACK(do_modify_error_on_iteration),
                         error);
        /**************************************************************/
        /**                   UNREF THE BUILDER                      **/
        /**************************************************************/
        gtk_builder_connect_signals(builder, NULL);
        g_object_unref(builder);
        /**************************************************************/
        /**             FORCING MENU BUTTON VISIBILITY               **/
        /**************************************************************/
        gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(window), FALSE);
        g_object_set (gtk_widget_get_settings(headerbar),
                      "gtk-shell-shows-app-menu", FALSE,
                      "gtk-decoration-layout", "menu:close",
                      NULL);
        /**************************************************************/
        /**             LINK THE WINDOW TO THE APPLICATION           **/
        /**************************************************************/
        gtk_application_add_window(GTK_APPLICATION(app), GTK_WINDOW(window));
        /**************************************************************/
        /**                 SETTINGS DEFAULT VALUES                  **/
        /**************************************************************/
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(error), 1);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), 0);
        gtk_widget_set_sensitive(restart, BRAIN_FALSE);
        /**************************************************************/
        /**                    SHOW ALL WIDGETS                      **/
        /**************************************************************/
        gtk_widget_show_all(window);
    }
}

/**********************************************************************/
/**                    DEFINING MAIN FUNCTION                        **/
/**********************************************************************/
BrainInt
main(BrainInt argc, BrainChar** argv)
{
    GtkApplication *app = NULL;
    BrainTrainer *trainer = NULL;
    GSettings* settings = NULL;
    BrainInt status;

    /******************************************************************/
    /**                     LOADING THE SETTINGS                     **/
    /******************************************************************/
    settings = g_settings_new("org.libbrain.config");
    GVariant* child = NULL;
    if (settings != NULL)
    {
        child = g_settings_get_value(settings, "datasets");
        printf("OK ON A OUVERT le settings\n");
    }
    /******************************************************************/
    /**                         ENABLE LOGGING                       **/
    /******************************************************************/
    brain_logging_init();
    /******************************************************************/
    /**                    CREATING THE TRAINER                      **/
    /******************************************************************/
    trainer = brain_trainer_new(argc, argv);
    /******************************************************************/
    /**                      CREATING THE APP                        **/
    /******************************************************************/
    app = gtk_application_new("org.brain.trainer", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activation), trainer);
    /******************************************************************/
    /**                        RUNNING APP                           **/
    /******************************************************************/
    status = g_application_run(G_APPLICATION(app), argc, argv);
    /******************************************************************/
    /**                    CLEANING EVERYTHING                       **/
    /******************************************************************/
    g_object_unref(app);
    g_object_unref(trainer);
    return status;
}
