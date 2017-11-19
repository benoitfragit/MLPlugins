#include "brain_plugin_manager.h"
#include "brain_plugin.h"
#include "brain_logging_utils.h"

#include <gtk/gtk.h>

static void
brain_quit(BrainPluginManager plugin_manager, gpointer data)
{
    /******************************************************************/
    /**                    DELETE THE PLUGIN MANAGER                 **/
    /******************************************************************/
    delete_plugin_manager(plugin_manager);
    /******************************************************************/
    /**                       LEAVE THE GTK LOOP                     **/
    /******************************************************************/
    gtk_main_quit();
}

static void
on_plugin_activation_change(BrainPluginManager manager, GParamSpec* pspec, gpointer data)
{
    if (manager)
    {

    }
}

static GtkWidget*
brain_window(BrainPluginManager manager)
{
    GtkWidget* mainwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_position(GTK_WINDOW(mainwindow), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_title(GTK_WINDOW(mainwindow), "Brain");
    gtk_window_set_default_size(GTK_WINDOW(mainwindow), 500, 350);
    gtk_container_set_border_width(GTK_CONTAINER(mainwindow), 5);

    g_signal_connect_swapped(mainwindow,
                             "delete-event",
                             G_CALLBACK(brain_quit),
                             manager);

    return mainwindow;
}

static void
brain_setup_popup(BrainPluginManager manager, GtkWidget* button)
{
    GtkWidget* popover      = gtk_popover_new(button);
    GtkWidget* plugins_grid = gtk_grid_new();
    BrainUint  row          = 0;

    if (manager)
    {
        const BrainUint number_of_plugins = get_number_of_plugins(manager);
        BrainUint i = 0;

        for (i = 0; i < number_of_plugins; ++i)
        {
            const BrainPlugin plugin = get_plugin_with_index(manager, i);

            if (plugin)
            {
                BrainString plugin_name = get_plugin_name(plugin);
                BrainBool   activated   = is_plugin_activated(manager, i);

                GtkWidget* plugin_label  = gtk_label_new(plugin_name);
                GtkWidget* plugin_switch = gtk_switch_new();
                GtkWidget* plugin_grid   = gtk_grid_new();

                gtk_switch_set_active(GTK_SWITCH(plugin_switch), FALSE);
                if (activated)
                {
                    gtk_switch_set_active(GTK_SWITCH(plugin_switch), TRUE);
                }

                gtk_widget_set_hexpand(plugin_label, TRUE);

                gtk_grid_attach(GTK_GRID(plugin_grid), plugin_label,  0, 0, 1, 1);
                gtk_grid_attach(GTK_GRID(plugin_grid), plugin_switch, 1, 0, 2, 1);

                gtk_widget_show_all(plugin_grid);

                gtk_grid_attach(GTK_GRID(plugins_grid), plugin_grid, 0, row, 1, 1);

                g_signal_connect_swapped(plugin_switch,
                                         "notify::active",
                                         G_CALLBACK(on_plugin_activation_change),
                                         manager);
                ++row;
            }
        }
    }

    gtk_container_add(GTK_CONTAINER(popover), plugins_grid);

    gtk_widget_show(plugins_grid);

    g_signal_connect_swapped(button,
                             "clicked",
                             G_CALLBACK(gtk_popover_popup),
                             popover);
}

static GtkWidget*
brain_setup_button(BrainPluginManager manager)
{
    GtkWidget* setupbutton = gtk_button_new_from_icon_name("document-properties", GTK_ICON_SIZE_SMALL_TOOLBAR);

    /******************************************************************/
    /**                  ATTACH A SETUP POPUP MENU                   **/
    /******************************************************************/
    brain_setup_popup(manager, setupbutton);

    gtk_button_set_relief(GTK_BUTTON(setupbutton),  GTK_RELIEF_NONE);

    return setupbutton;
}

static GtkWidget*
brain_new_button()
{
    GtkWidget* newbutton  = gtk_button_new_from_icon_name("list-add", GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_button_set_relief(GTK_BUTTON(newbutton),    GTK_RELIEF_NONE);
    return newbutton;
}

static GtkWidget*
brain_header(BrainPluginManager manager)
{
    GtkWidget* headerbar  = gtk_header_bar_new();
    /******************************************************************/
    /**                       CREATE A NEW BUTTON                    **/
    /******************************************************************/
    GtkWidget* newbutton = brain_new_button();
    /******************************************************************/
    /**                      CREATE A SETUP BUTTON                   **/
    /******************************************************************/
    GtkWidget* setupbutton = brain_setup_button(manager);
    /******************************************************************/
    /**                    CONFIGURE THE HEADER BAR                  **/
    /******************************************************************/
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(headerbar), BRAIN_TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar), "Brain");
    gtk_header_bar_pack_start(GTK_HEADER_BAR(headerbar), newbutton);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(headerbar), setupbutton);

    return headerbar;
}

BrainInt
main(BrainInt argc, BrainChar** argv)
{
    BrainInt ret                = EXIT_SUCCESS;
    /******************************************************************/
    /**                    CREATE A PLUGIN MANAGER                   **/
    /******************************************************************/
    BrainPluginManager manager = new_plugin_manager();
    /******************************************************************/
    /**                  INITIALISE THE GTK LAYER                    **/
    /******************************************************************/
    gtk_init(&argc, &argv);
    /******************************************************************/
    /**                   CREATE THE MAIN WINDOW                     **/
    /******************************************************************/
    GtkWidget* mainwindow = brain_window(manager);
    /******************************************************************/
    /**                   CREATE THE HEADER BAR                      **/
    /******************************************************************/
    GtkWidget* headerbar = brain_header(manager);
    /******************************************************************/
    /**                  POPULATE THE MAIN WINDOW                    **/
    /******************************************************************/
    gtk_window_set_titlebar(GTK_WINDOW(mainwindow), headerbar);
    /******************************************************************/
    /**                      SHOW THE MAIN WINDOW                    **/
    /******************************************************************/
    gtk_widget_show_all(mainwindow);
    /******************************************************************/
    /**                      START THE MAIN LOOP                    **/
    /******************************************************************/
    gtk_main();
    return ret;
}
