#include "brain_plugin_manager.h"
#include "brain_plugin.h"
#include "brain_logging_utils.h"

#include <gtk/gtk.h>
#include <glib.h>

typedef struct View
{
    GtkWidget*          _window;
    GtkWidget*          _iconview;
    BrainPluginManager  _manager;
    GHashTable*         _fromSwitch;
} View;

typedef View* BrainView;

static void
brain_quit(BrainView view, gpointer data)
{
    if (view)
    {
        g_hash_table_destroy(view->_fromSwitch);
        /**************************************************************/
        /**                    DELETE THE PLUGIN MANAGER             **/
        /**************************************************************/
        delete_plugin_manager(view->_manager);
    }
    /******************************************************************/
    /**                       LEAVE THE GTK LOOP                     **/
    /******************************************************************/
    gtk_main_quit();
}

static void
on_plugin_activation_change(BrainView view, GParamSpec* pspec, gpointer data)
{
    GtkSwitch* plugin_switch = GTK_SWITCH(data);

    if (view && plugin_switch)
    {
        BrainPlugin plugin = (BrainPlugin)g_hash_table_lookup(view->_fromSwitch, plugin_switch);

        if (plugin)
        {
            if (gtk_switch_get_active(plugin_switch))
            {
                set_plugin_activated(view->_manager, plugin, BRAIN_TRUE);
            }
            else
            {
                set_plugin_activated(view->_manager, plugin, BRAIN_FALSE);
            }
        }
    }
}

static void
brain_attach_setup_popup(BrainView view, GtkWidget* button)
{
    if (view &&
        button)
    {
        GtkWidget* popover      = gtk_popover_new(button);
        GtkWidget* plugins_grid = gtk_grid_new();
        BrainUint  row          = 0;

        const BrainUint number_of_plugins = get_number_of_plugins(view->_manager);
        BrainUint i = 0;

        for (i = 0; i < number_of_plugins; ++i)
        {
            const BrainPlugin plugin = get_plugin_with_index(view->_manager, i);

            if (plugin)
            {
                BrainString plugin_name = get_plugin_name(plugin);
                BrainBool   activated   = is_plugin_activated(view->_manager, i);

                GtkWidget* plugin_label  = gtk_label_new(plugin_name);
                GtkWidget* plugin_switch = gtk_switch_new();
                GtkWidget* plugin_grid   = gtk_grid_new();
                /******************************************************/
                /**               INITIALIZE THE SWITCH              **/
                /******************************************************/
                gtk_switch_set_active(GTK_SWITCH(plugin_switch), FALSE);
                if (activated)
                {
                    gtk_switch_set_active(GTK_SWITCH(plugin_switch), TRUE);
                }
                /******************************************************/
                /**                   MANAGE THE LAYER               **/
                /******************************************************/
                gtk_widget_set_hexpand(plugin_label, TRUE);
                gtk_grid_attach(GTK_GRID(plugin_grid), plugin_label,  0, 0, 1, 1);
                gtk_grid_attach(GTK_GRID(plugin_grid), plugin_switch, 1, 0, 2, 1);
                gtk_grid_attach(GTK_GRID(plugins_grid), plugin_grid, 0, row, 1, 1);
                gtk_widget_show_all(plugin_grid);
                /******************************************************/
                /**        RECEIVE A STATE CHANGED NOTIFICATION      **/
                /******************************************************/
                g_signal_connect_swapped(plugin_switch,
                                         "notify::active",
                                         G_CALLBACK(on_plugin_activation_change),
                                         view);
                /******************************************************/
                /**       KEEP A LINK BETWEEN SWITCH AND PLUGIN      **/
                /******************************************************/
                g_hash_table_insert(view->_fromSwitch,
                                    plugin_switch,
                                    (gpointer)plugin);

                ++row;
            }
        }

        gtk_container_add(GTK_CONTAINER(popover), plugins_grid);
        gtk_widget_show(plugins_grid);

        g_signal_connect_swapped(button,
                                 "clicked",
                                 G_CALLBACK(gtk_popover_popup),
                                 popover);
    }
}

static GtkWidget*
brain_create_setup_button(BrainView view)
{
    GtkWidget* setupbutton = gtk_button_new_from_icon_name("document-properties", GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_button_set_relief(GTK_BUTTON(setupbutton),  GTK_RELIEF_NONE);
    /******************************************************************/
    /**                  ATTACH A SETUP POPUP MENU                   **/
    /******************************************************************/
    brain_attach_setup_popup(view, setupbutton);
    return setupbutton;
}

static GtkWidget*
brain_create_new_button()
{
    GtkWidget* newbutton  = gtk_button_new_from_icon_name("list-add", GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_button_set_relief(GTK_BUTTON(newbutton), GTK_RELIEF_NONE);
    return newbutton;
}

static void
brain_create_header(BrainView view)
{
    if (view)
    {
        GtkWidget* headerbar  = gtk_header_bar_new();
        /**************************************************************/
        /**                     CREATE A NEW BUTTON                  **/
        /**************************************************************/
        GtkWidget* newbutton = brain_create_new_button();
        /**************************************************************/
        /**                    CREATE A SETUP BUTTON                 **/
        /**************************************************************/
        GtkWidget* setupbutton = brain_create_setup_button(view);
        /**************************************************************/
        /**                  CONFIGURE THE HEADER BAR                **/
        /**************************************************************/
        gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(headerbar), TRUE);
        gtk_header_bar_set_title(GTK_HEADER_BAR(headerbar), "Brain");
        gtk_header_bar_pack_start(GTK_HEADER_BAR(headerbar), newbutton);
        gtk_header_bar_pack_end(GTK_HEADER_BAR(headerbar), setupbutton);
        /**************************************************************/
        /**                  APPEND THE HEADER BAR                   **/
        /**************************************************************/
        gtk_window_set_titlebar(GTK_WINDOW(view->_window), headerbar);
    }
}

static void
brain_create_iconview(BrainView view)
{
    GtkTreeIter iter;
    GError* err = NULL;

    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file("/home/benoit/Documents/projets/C/libBrain/view/src/package.png", &err);
    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL,NULL);
    GtkWidget* iconview = gtk_icon_view_new();
    GtkListStore* store = gtk_list_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "package", 1, pixbuf, -1);
    gtk_icon_view_set_model(GTK_ICON_VIEW(iconview), GTK_TREE_MODEL(store));
    gtk_icon_view_set_text_column(GTK_ICON_VIEW(iconview), 0);
    gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(iconview), 1);

    gtk_container_add(GTK_CONTAINER(scrolled_window), iconview);
    gtk_container_add(GTK_CONTAINER(view->_window), scrolled_window);

    gtk_widget_show_all(scrolled_window);
}

static void
brain_create_window(BrainView view)
{
    if (view)
    {
        /**************************************************************/
        /**                   BUILD THE WINDOW                       **/
        /**************************************************************/
        view->_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_position(GTK_WINDOW(view->_window), GTK_WIN_POS_CENTER_ALWAYS);
        gtk_window_set_title(GTK_WINDOW(view->_window), "Brain");
        gtk_window_set_default_size(GTK_WINDOW(view->_window), 500, 350);
        gtk_container_set_border_width(GTK_CONTAINER(view->_window), 5);
        /**************************************************************/
        /**               CONNECT THE DESTROY SIGNAL                 **/
        /**************************************************************/
        g_signal_connect_swapped(view->_window,
                                 "delete-event",
                                 G_CALLBACK(brain_quit),
                                 view);
        /**************************************************************/
        /**                  CREATE THE HEADER BAR                   **/
        /**************************************************************/
        brain_create_header(view);
        /**************************************************************/
        /**                  CREATE THE MAIN VIEW                    **/
        /**************************************************************/
        brain_create_iconview(view);
    }
}

BrainInt
main(BrainInt argc, BrainChar** argv)
{
    BrainInt ret = EXIT_SUCCESS;
    /******************************************************************/
    /**                  INITIALISE THE GTK LAYER                    **/
    /******************************************************************/
    gtk_init(&argc, &argv);
    /******************************************************************/
    /**                    CREATE A BRAINVIEW                        **/
    /******************************************************************/
    View view;
    /******************************************************************/
    /**                    CREATE A PLUGIN MANAGER                   **/
    /******************************************************************/
    view._manager = new_plugin_manager();
    /******************************************************************/
    /**                    CREATE A PLUGIN HASHTABLE                 **/
    /******************************************************************/
    view._fromSwitch = g_hash_table_new(g_direct_hash, g_direct_equal);
    /******************************************************************/
    /**                   CREATE THE MAIN WINDOW                     **/
    /******************************************************************/
    brain_create_window(&(view));
    /******************************************************************/
    /**                      SHOW THE MAIN WINDOW                    **/
    /******************************************************************/
    gtk_widget_show_all(view._window);
    /******************************************************************/
    /**                      START THE MAIN LOOP                    **/
    /******************************************************************/
    gtk_main();
    return ret;
}
