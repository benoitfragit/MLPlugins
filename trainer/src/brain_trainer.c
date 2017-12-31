#include "brain_plugin.h"
#include "brain_logging_utils.h"
#include "brain_memory_utils.h"
#include "brain_core_types.h"

static BrainChar* _trainer_args[][2] =
{
    {"-p","--plugin="},
    {"-n","--network="},
    {"-s","--settings="},
    {"-d","--data="},
    {"-r","--record="},
    {"-h","--help"}
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
    printf("This program is a Neural Network generic trainer. In order\n");
    printf("to train your network you should specify some parameters:\n");
    printf("[-p, --plugin] To load a Network plugin\n");
    printf("[-n, --network] To load a Network\n");
    printf("[-s, --settings] To load network's settings\n");
    printf("[-d, --data] To load training data\n");
    printf("[-r, --record] To set the recording path\n");
    printf("[-h, --help] To display this help\n");
}

/**********************************************************************/
/**                    DEFINING MAIN FUNCTION                        **/
/**********************************************************************/
BrainInt
main(BrainInt argc, BrainChar** argv)
{
    BrainInt i = 0;
    BrainUint j = 0;
    BrainChar args[5][400];
    BrainBool valid[5];
    if (2 <= argc)
    {
        BrainPlugin plugin = NULL;
        BRAIN_SET(valid, BRAIN_FALSE, BrainBool, 5);
        /**************************************************************/
        /**                     PARSING CMD LINE ARGS                **/
        /**************************************************************/
        for (i = 0; i < argc; ++i)
        {
            for (j = First_Args; j <= Last_Args; ++j)
            {
                if (!strcmp(argv[i], _trainer_args[j][0]))
                {
                    if (j != Help)
                    {
                        strcpy(args[j], argv[i + 1]);
                        valid[j] = BRAIN_TRUE;
                    }
                    else
                    {
                        help();
                    }
                    break;
                }
                else
                {
                    if (strstr(argv[i], _trainer_args[j][1]) != NULL)
                    {
                        if (j != Help)
                        {
                            BrainChar* buffer = NULL;

                            buffer = strtok(argv[i], "=");
                            buffer = strtok(NULL, "=");

                            if (BRAIN_ALLOCATED(buffer))
                            {
                                strcpy(args[j], buffer);
                                valid[j] = BRAIN_TRUE;
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
        if (valid[PluginName])
        {
            plugin = new_plugin(args[PluginName]);
            if (BRAIN_ALLOCATED(plugin))
            {
                /**********************************************************/
                /**                      LOADING THE NETWORK             **/
                /**********************************************************/
                if (valid[NetworkPath])
                {
                    BrainNetwork network = new_plugin_network(plugin, args[NetworkPath]);
                    if (BRAIN_ALLOCATED(network))
                    {
                        /******************************************************/
                        /**                   TWEAKING THE NETWORK           **/
                        /******************************************************/
                        if (valid[SettingsPath])
                        {
                            configure_network(plugin, network, args[SettingsPath]);
                        }
                        /******************************************************/
                        /**                   TRAINING THE NETWORK           **/
                        /******************************************************/
                        if (valid[DataPath])
                        {
                            train_network(plugin, network, args[DataPath], ",", BRAIN_TRUE, BRAIN_FALSE);
                            /******************************************************/
                            /**                     SAVE THE NETWORK             **/
                            /******************************************************/
                            if (valid[RecordPath])
                            {
                                serialize_network(plugin, network, args[RecordPath]);
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
