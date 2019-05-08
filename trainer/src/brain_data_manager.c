#include "brain_data_manager.h"
#include "brain_data_utils.h"
#include "brain_plugin.h"
#include "brain_logging_utils.h"
#include "brain_memory_utils.h"
#include "brain_core_types.h"
#include "trainer_config.h"

#define BRAIN_DATA_FORMAT "siiysssss"
#define BRAIN_DATASET_FORMAT "a(siiysssss)"

#define ACCESS_PRIVATE_MEMBERS(manager) manager->priv = G_TYPE_INSTANCE_GET_PRIVATE (manager, TYPE_BRAIN_DATA_MANAGER, BrainDataManagerPrivate);\
                                        BrainDataManagerPrivate *priv = manager->priv;

/**********************************************************************/
/**                        SIGNALS DEFINITION                        **/
/**********************************************************************/
enum
{
    BRAIN_DATA_MANAGER_SIGNAL_LAST
};

static BrainUint brain_data_manager_signals[BRAIN_DATA_MANAGER_SIGNAL_LAST];
/**********************************************************************/
/**                    PROPERTY ENUM DEFINITION                      **/
/**********************************************************************/
enum
{
    PROP_O,
    PROP_ACTIVATED_DATA
};
/**********************************************************************/
/**               DEFINING A PRIVATE BRAIN DATA MANAGER              **/
/**********************************************************************/
typedef struct _BrainDataManagerPrivate
{
    /******************************************************************/
    /**                         CORE STRUCTURE                       **/
    /******************************************************************/
    GHashTable *_datas;
    BrainData   _activated;
    gchar*      _activated_name;
} _BrainDataManagerPrivate;
/**********************************************************************/
/**                  DEFINING BRAIN_DATA_MANAGER TYPE                     **/
/**********************************************************************/
G_DEFINE_TYPE (BrainDataManager, brain_data_manager, G_TYPE_OBJECT);

void
brain_data_manager_deactivate_data(BrainDataManager* manager)
{
    if (BRAIN_ALLOCATED(manager))
    {
        ACCESS_PRIVATE_MEMBERS(manager)

        if (BRAIN_ALLOCATED(priv))
        {
            if (BRAIN_ALLOCATED(priv->_activated_name))
            {
                g_free(priv->_activated_name);
            }

            if (BRAIN_ALLOCATED(priv->_activated))
            {
                free(priv->_activated);
            }
        }
    }
}

static void
brain_data_manager_activate_data(BrainDataManager* manager, const gchar* activated_data_name)
{
    if (BRAIN_ALLOCATED(manager) &&
        BRAIN_ALLOCATED(activated_data_name))
    {
        ACCESS_PRIVATE_MEMBERS(manager)

        if (BRAIN_ALLOCATED(priv) &&
            BRAIN_ALLOCATED(priv->_datas))
        {
            if (g_hash_table_contains(priv->_datas, activated_data_name))
            {
                BrainDataParameters parameters = NULL;
                // Remove the previous loaded data
                brain_data_manager_deactivate_data(manager);
                // Record the activated name
                priv->_activated_name = g_strdup(activated_data_name);
                // Get associated parameters
                parameters = (BrainDataParameters)g_hash_table_lookup(priv->_datas, activated_data_name);
                // Load the corresponding data
                priv->_activated = new_data_with_parameters(parameters);
            }
        }
    }
}

static void
brain_data_manager_dispose(GObject *object)
{
    G_OBJECT_CLASS (brain_data_manager_parent_class)->dispose (object);
}

static void
brain_data_manager_finalize(GObject *object)
{
    BrainDataManager *manager = BRAIN_DATA_MANAGER(object);

    ACCESS_PRIVATE_MEMBERS(manager)

    if (BRAIN_ALLOCATED(priv))
    {
        g_hash_table_destroy(priv->_datas);
    }

    G_OBJECT_CLASS (brain_data_manager_parent_class)->finalize (object);
}

static void
brain_data_manager_set_property (GObject      *gobject,
                      guint         prop_id,
                      const GValue *value,
                      GParamSpec   *pspec)
{
    BrainDataManager* manager = BRAIN_DATA_MANAGER(gobject);

    if (BRAIN_ALLOCATED(manager))
    {
        ACCESS_PRIVATE_MEMBERS(manager);

        switch (prop_id)
        {
            case PROP_ACTIVATED_DATA:
            {
                const gchar *activated_data_name = (gchar *)g_value_get_string(value);
                brain_data_manager_activate_data(manager, activated_data_name);
            }
                break;
            default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
                break;
        }
    }
}

static void
brain_data_manager_get_property (GObject    *gobject,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
    BrainDataManager* manager = BRAIN_DATA_MANAGER(gobject);

    if (BRAIN_ALLOCATED(manager))
    {
        ACCESS_PRIVATE_MEMBERS(manager);

        if (BRAIN_ALLOCATED(priv))
        {
            switch (prop_id)
            {
                case PROP_ACTIVATED_DATA:
                    g_value_set_string(value, priv->_activated_name);
                    break;
                default:
                    G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
                    break;
            }
        }
    }
}

static void
brain_data_manager_class_init(BrainDataManagerClass *klass)
{
    g_type_class_add_private (klass, sizeof (BrainDataManagerPrivate));
    /******************************************************************/
    /**             PASSING ALL REQUIRED FUNCTION POINTERS           **/
    /******************************************************************/
    GObjectClass *gklass = G_OBJECT_CLASS(klass);
    gklass->get_property = brain_data_manager_get_property;
    gklass->set_property = brain_data_manager_set_property;
    gklass->dispose      = brain_data_manager_dispose;
    gklass->finalize     = brain_data_manager_finalize;
    /******************************************************************/
    /**                  INSTALLING ALL PROPERTIES                   **/
    /******************************************************************/
    GParamSpec *pspec = g_param_spec_string ("activated-data",
                                             "activated-data",
                                             "activated-data",
                                             NULL,
                                             G_PARAM_READWRITE);
    g_object_class_install_property (gklass, PROP_ACTIVATED_DATA, pspec);
}

static void
brain_data_manager_init(BrainDataManager* manager)
{
    if (BRAIN_ALLOCATED(manager))
    {
        ACCESS_PRIVATE_MEMBERS(manager)

        if (BRAIN_ALLOCATED(priv))
        {
            priv->_activated = NULL;
            priv->_activated_name = NULL;
            priv->_datas = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
        }
    }
}

BrainDataManager*
brain_data_manager_new()
{
    /******************************************************************/
    /**                     CREATE A DATA MANAGER                    **/
    /******************************************************************/
    BrainDataManager* manager = (BrainDataManager *)g_object_new(TYPE_BRAIN_DATA_MANAGER, NULL);

    ACCESS_PRIVATE_MEMBERS(manager)

    if (BRAIN_ALLOCATED(priv))
    {
        /**********************************************************/
        /**                   CREATE A PLUGIN                    **/
        /**********************************************************/
        priv->_activated_name  = NULL;
        /**********************************************************/
        /**                  LOADING THE NETWORK                 **/
        /**********************************************************/
        priv->_activated = NULL;
        /*********************************************************/
        /**                      LOAD THE DATA                  **/
        /*********************************************************/
        priv->_datas = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    }

    return manager;
}

void
brain_data_manager_add_new_data(BrainDataManager* manager, gchar* name, BrainDataParameters parameters)
{
    if (BRAIN_ALLOCATED(manager) &&
        BRAIN_ALLOCATED(name) &&
        BRAIN_ALLOCATED(parameters))
    {
        ACCESS_PRIVATE_MEMBERS(manager)

        if (BRAIN_ALLOCATED(priv))
        {
            if (!g_hash_table_contains(priv->_datas, name))
            {
                g_hash_table_insert(priv->_datas, name, parameters);
            }
            else
            {
                g_hash_table_replace(priv->_datas, name, parameters);
            }
        }
    }
}

BrainData
brain_data_manager_get_activated_data(BrainDataManager* manager)
{
    BrainData data = NULL;

    if (BRAIN_ALLOCATED(manager))
    {
        ACCESS_PRIVATE_MEMBERS(manager)

        if (BRAIN_ALLOCATED(priv))
        {
            data = priv->_activated;
        }
    }

    return data;
}

BrainDataParameters
brain_data_manager_get_parameters(BrainDataManager* manager, BrainString name)
{
    BrainDataParameters parameters = NULL;

    if (BRAIN_ALLOCATED(manager) &&
        BRAIN_ALLOCATED(name))
    {
        ACCESS_PRIVATE_MEMBERS(manager)

        if (BRAIN_ALLOCATED(priv))
        {
            if (g_hash_table_contains(priv->_datas, name))
            {
                parameters = (BrainDataParameters)g_hash_table_lookup(priv->_datas, name);
            }
        }
    }

    return parameters;
}

void
brain_data_manager_from_variant(BrainDataManager* manager, GVariant* list)
{
    GVariantIter* iter = NULL;

    if (BRAIN_ALLOCATED(list) &&
        BRAIN_ALLOCATED(manager))
    {
        ACCESS_PRIVATE_MEMBERS(manager)

        if (BRAIN_ALLOCATED(priv))
        {
            BrainDataParameters parameters = g_malloc0(1*sizeof(DataParameters));
            gchar*          name       = NULL;

            //get the content of the GVariant
            g_variant_get(list, BRAIN_DATASET_FORMAT, &iter);

            while(g_variant_iter_loop(  iter,
                                        BRAIN_DATA_FORMAT,
                                        &name,
                                        &(parameters->input_length),
                                        &(parameters->output_length),
                                        &(parameters->is_labedelled),
                                        &(parameters->format),
                                        &(parameters->parser),
                                        &(parameters->tokenizer),
                                        &(parameters->repository_path),
                                        &(parameters->preprocessing)))
            {
                brain_data_manager_add_new_data(manager, name, parameters);
            }

            g_variant_iter_free(iter);
        }
    }
}

static void
brain_data_manager_dataset_to_variant(gpointer key, gpointer value, gpointer user_data)
{
    if (BRAIN_ALLOCATED(key) &&
        BRAIN_ALLOCATED(value) &&
        BRAIN_ALLOCATED(user_data))
    {
        gchar* name = (gchar *)key;
        BrainDataParameters parameters = (BrainDataParameters)value;
        GVariantBuilder* builder = (GVariantBuilder *)user_data;

        g_variant_builder_add(builder,
                              BRAIN_DATA_FORMAT,
                              name,
                              parameters->input_length,
                              parameters->output_length,
                              parameters->is_labedelled,
                              parameters->format,
                              parameters->parser,
                              parameters->tokenizer,
                              parameters->repository_path,
                              parameters->preprocessing);
    }
}

GVariant*
brain_data_manager_to_variant(BrainDataManager* manager)
{
    GVariant* value = NULL;

    if (BRAIN_ALLOCATED(manager))
    {
        ACCESS_PRIVATE_MEMBERS(manager)

        if (BRAIN_ALLOCATED(priv))
        {
            GVariantBuilder* builder = g_variant_builder_new(G_VARIANT_TYPE(BRAIN_DATASET_FORMAT));

            g_hash_table_foreach(priv->_datas,
                                 brain_data_manager_dataset_to_variant,
                                 builder);

            value = g_variant_new(BRAIN_DATASET_FORMAT, builder);
            g_variant_builder_unref(builder);
        }
    }

    return value;
}
