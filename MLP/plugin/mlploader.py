#!/usr/bin/env python
# -*- coding: utf-8 -*-

from core import MLLoader

from exchange import MLPTrainer
from exchange import MLPNetwork
from exchange import MLPMetaData

import ctypes

def enum(*args):
    values = dict(zip(args, range(len(args))))
    return type('Enum', (), values)

class MLPFunction:
    INIT                                = 0
    METADATA                            = 1
    TRAINER_NEW                         = 2
    TRAINER_DELETE                      = 3
    TRAINER_CONFIGURE                   = 4
    TRAINER_IS_RUNNING                  = 5
    TRAINER_GET_PROGRESS                = 6
    TRAINER_RUN                         = 7
    TRAINER_ERROR                       = 8
    TRAINER_SAVE_PROGRESSION            = 9
    TRAINER_RESTORE_PROGRESSION         = 10
    TRAINER_GET_MANAGED_NETWORK         = 11
    NETWORK_NEW                         = 12
    NETWORK_DELETE                      = 13
    NETWORK_SERIALIZE                   = 14
    NETWORK_DESERIALIZE                 = 15
    NETWORK_PREDICT                     = 16
    NETWORK_GET_OUTPUT                  = 17
    NETWORK_GET_OUTPUT_LENGTH           = 18
    NETWORK_LAYER_GET_NUMBER_OF_NEURON  = 19
    NETWORK_GET_NUMBER_OF_LAYER         = 20
    NETWORK_GET_NUMBER_OF_INPUT         = 21

class MLPLoader(MLLoader):
    def __init__(self):
        MLLoader.__init__(self, 'libMLP.so')

        self._api = {}

        self._api[MLPFunction.INIT]                                 = ('mlp_plugin_init',                         None,                       [])
        self._api[MLPFunction.METADATA]                             = ('mlp_plugin_metadata',                     ctypes.POINTER(MLPMetaData),[])
        self._api[MLPFunction.TRAINER_NEW]                          = ('mlp_trainer_new',                         ctypes.POINTER(MLPTrainer), [ctypes.c_char_p, ctypes.c_char_p])
        self._api[MLPFunction.TRAINER_DELETE]                       = ('mlp_trainer_delete',                      None,                       [ctypes.POINTER(MLPTrainer)])
        self._api[MLPFunction.TRAINER_CONFIGURE]                    = ('mlp_trainer_configure',                   None,                       [ctypes.POINTER(MLPTrainer), ctypes.c_char_p])
        self._api[MLPFunction.TRAINER_IS_RUNNING]                   = ('mlp_trainer_is_running',                  ctypes.c_ubyte,             [ctypes.POINTER(MLPTrainer)])
        self._api[MLPFunction.TRAINER_GET_PROGRESS]                 = ('mlp_trainer_get_progress',                ctypes.c_double,            [ctypes.POINTER(MLPTrainer)])
        self._api[MLPFunction.TRAINER_RUN]                          = ('mlp_trainer_run',                         None,                       [ctypes.POINTER(MLPTrainer)])
        self._api[MLPFunction.TRAINER_ERROR]                        = ('mlp_trainer_error',                       ctypes.c_double,            [ctypes.POINTER(MLPTrainer)])
        self._api[MLPFunction.TRAINER_SAVE_PROGRESSION]             = ('mlp_trainer_save_progression',            None,                       [ctypes.POINTER(MLPTrainer), ctypes.c_char_p])
        self._api[MLPFunction.TRAINER_RESTORE_PROGRESSION]          = ('mlp_trainer_restore_progression',         None,                       [ctypes.POINTER(MLPTrainer), ctypes.c_char_p, ctypes.c_double, ctypes.c_double])
        self._api[MLPFunction.TRAINER_GET_MANAGED_NETWORK]          = ('mlp_trainer_get_network',                 ctypes.POINTER(MLPNetwork), [ctypes.POINTER(MLPTrainer)])
        self._api[MLPFunction.NETWORK_NEW]                          = ('mlp_network_new',                         ctypes.POINTER(MLPNetwork), [ctypes.c_char_p])
        self._api[MLPFunction.NETWORK_DELETE]                       = ('mlp_network_delete',                      None,                       [ctypes.POINTER(MLPNetwork)])
        self._api[MLPFunction.NETWORK_SERIALIZE]                    = ('mlp_network_serialize',                   None,                       [ctypes.POINTER(MLPNetwork), ctypes.c_char_p])
        self._api[MLPFunction.NETWORK_DESERIALIZE]                  = ('mlp_network_deserialize',                 None,                       [ctypes.POINTER(MLPNetwork), ctypes.c_char_p])
        self._api[MLPFunction.NETWORK_PREDICT]                      = ('mlp_network_predict',                     None,                       [ctypes.POINTER(MLPNetwork), ctypes.c_uint, ctypes.c_void_p])
        self._api[MLPFunction.NETWORK_GET_OUTPUT]                   = ('mlp_network_get_output',                  ctypes.c_void_p,            [ctypes.POINTER(MLPNetwork)])
        self._api[MLPFunction.NETWORK_GET_OUTPUT_LENGTH]            = ('mlp_network_get_output_length',           ctypes.c_uint,              [ctypes.POINTER(MLPNetwork)])
        self._api[MLPFunction.NETWORK_LAYER_GET_NUMBER_OF_NEURON]   = ('mlp_network_get_layer_number_of_neuron',  ctypes.c_uint,              [ctypes.POINTER(MLPNetwork), ctypes.c_uint])
        self._api[MLPFunction.NETWORK_GET_NUMBER_OF_LAYER]          = ('mlp_network_get_number_of_layer',         ctypes.c_uint,              [ctypes.POINTER(MLPNetwork)])
        self._api[MLPFunction.NETWORK_GET_NUMBER_OF_INPUT]          = ('mlp_network_get_number_of_input',         ctypes.c_uint,              [ctypes.POINTER(MLPNetwork)])

        self._funcs = {}

        self.load()

    def load(self):
        for f in self._api.keys():
            api = self._api[f]
            self._funcs[f] = self.wrap(api[0], api[1], api[2])

            if self._funcs[f] is None:
                print >> sys.stderr, 'Method:' + api[0] + ' hasn t been loaded'
