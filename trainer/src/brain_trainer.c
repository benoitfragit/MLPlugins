#include "brain_plugin.h"
#include "brain_logging_utils.h"
#include "brain_memory_utils.h"
#include "brain_core_types.h"

typedef struct TrainerCmd
{
    BrainString _short_name;
    BrainString _full_name;
    BrainString _description;
    BrainBool   _valid;
    BrainChar   _args[300];
} TrainerCmd;

static TrainerCmd _trainer_cmds[] =
{
    {"-p","--plugin=",  "Load a plugin",            BRAIN_FALSE, ""},
    {"-n","--network=", "Load a network",           BRAIN_FALSE, ""},
    {"-s","--settings=","Load settings",            BRAIN_FALSE, ""},
    {"-d","--data=",    "Load data",                BRAIN_FALSE, ""},
    {"-r","--record=",  "Set the recording path",   BRAIN_FALSE, ""},
    {"-h","--help",     "Display help message",     BRAIN_TRUE,  ""}
};

typedef enum BrainTrainerArgs
{
    PluginName,
    NetworkPath,
    SettingsPath,
    DataPath,
    RecordPath,
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
        printf("[%s, %s] %s\n",_trainer_cmds[i]._short_name, _trainer_cmds[i]._full_name, _trainer_cmds[i]._description);
    }
}

/**********************************************************************/
/**                    DEFINING MAIN FUNCTION                        **/
/**********************************************************************/
BrainInt
main(BrainInt argc, BrainChar** argv)
{
    BrainInt i = 0;
    BrainUint j = 0;
    if (2 <= argc)
    {
        BrainPlugin plugin = NULL;
        /**************************************************************/
        /**                     PARSING CMD LINE ARGS                **/
        /**************************************************************/
        for (i = 0; i < argc; ++i)
        {
            for (j = First_Args; j <= Last_Args; ++j)
            {
                if (!strcmp(argv[i], _trainer_cmds[j]._short_name))
                {
                    if (j != Help)
                    {
                        strcpy(_trainer_cmds[j]._args, argv[i + 1]);
                        _trainer_cmds[j]._valid = BRAIN_TRUE;
                    }
                    else
                    {
                        help();
                    }
                    break;
                }
                else
                {
                    if (strstr(argv[i], _trainer_cmds[j]._full_name) != NULL)
                    {
                        if (j != Help)
                        {
                            BrainChar* buffer = NULL;

                            buffer = strtok(argv[i], "=");
                            buffer = strtok(NULL, "=");

                            if (BRAIN_ALLOCATED(buffer))
                            {
                                strcpy(_trainer_cmds[j]._args, buffer);
                                _trainer_cmds[j]._valid = BRAIN_TRUE;
                            }
                        }
                        else
                        {
                            help();
                        }
                        break;
                    }
                }
            }
        }
        /**************************************************************/
        /**                      LOADING THE PLUGIN                  **/
        /**************************************************************/
        if (_trainer_cmds[PluginName]._valid)
        {
            plugin = new_plugin(_trainer_cmds[PluginName]._args);
            if (BRAIN_ALLOCATED(plugin))
            {
                /**********************************************************/
                /**                      LOADING THE NETWORK             **/
                /**********************************************************/
                if (_trainer_cmds[NetworkPath]._valid)
                {
                    BrainNetwork network = new_plugin_network(plugin, _trainer_cmds[NetworkPath]._args);
                    if (BRAIN_ALLOCATED(network))
                    {
                        /******************************************************/
                        /**                   TWEAKING THE NETWORK           **/
                        /******************************************************/
                        if (_trainer_cmds[SettingsPath]._valid)
                        {
                            configure_network(plugin, network, _trainer_cmds[SettingsPath]._args);
                        }
                        /******************************************************/
                        /**                   TRAINING THE NETWORK           **/
                        /******************************************************/
                        if (_trainer_cmds[DataPath]._valid)
                        {
                            train_network(plugin, network, _trainer_cmds[DataPath]._args);
                            /******************************************************/
                            /**                     SAVE THE NETWORK             **/
                            /******************************************************/
                            if (_trainer_cmds[RecordPath]._valid)
                            {
                                serialize_network(plugin, network, _trainer_cmds[RecordPath]._args);
                            }
                        }
                        /******************************************************/
                        /**                   CLEANING THE NETWORK           **/
                        /******************************************************/
                        delete_network(plugin, network);
                    }
                }
                /**********************************************************/
                /**                      CLEANING THE PLUGIN             **/
                /**********************************************************/
                delete_plugin(plugin);
            }
        }
    }
    return EXIT_SUCCESS;
}
