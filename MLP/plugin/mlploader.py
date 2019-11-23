#!/usr/bin/env python
# -*- coding: utf-8 -*-

from core import MLLoader
from core import MLFunction

from exchange import MLPTrainer
from exchange import MLPNetwork
from exchange import MLPMetaData

import ctypes

def enum(*args):
    values = dict(zip(args, range(len(args))))
    return type('Enum', (), values)

class MLPLoader(MLLoader):
    init                            = None
    metadata                        = None
    newTrainer                      = None
    deleteTrainer                   = None
    configureTrainer                = None
    isTrainerRunning                = None
    getTrainerProgress              = None
    runTrainer                      = None
    getTrainerError                 = None
    saveTrainerProgress             = None
    restoreTrainerProgress          = None
    getTrainerManagedNetwork        = None
    newNetwork                      = None
    deleteNetwork                   = None
    serializeNetwork                = None
    deserializeNetwork              = None
    predict                         = None
    getNetworkOutput                = None
    getNetworkOutputLength          = None
    getNetworkLayerNumberOfNeuron   = None
    getNetworkNumberOfLayer         = None
    getNetworkNumberOfInput         = None

    def __init__(self):
        MLLoader.__init__(self, 'libMLP.so')

        self.init                            = MLFunction(self, 'mlp_plugin_init',                         None,                       [])
        self.metadata                        = MLFunction(self, 'mlp_plugin_metadata',                     ctypes.POINTER(MLPMetaData),[])
        self.newTrainer                      = MLFunction(self, 'mlp_trainer_new',                         ctypes.POINTER(MLPTrainer), [ctypes.c_char_p, ctypes.c_char_p])
        self.deleteTrainer                   = MLFunction(self, 'mlp_trainer_delete',                      None,                       [ctypes.POINTER(MLPTrainer)])
        self.configureTrainer                = MLFunction(self, 'mlp_trainer_configure',                   None,                       [ctypes.POINTER(MLPTrainer), ctypes.c_char_p])
        self.isTrainerRunning                = MLFunction(self, 'mlp_trainer_is_running',                  ctypes.c_ubyte,             [ctypes.POINTER(MLPTrainer)])
        self.getTrainerProgress              = MLFunction(self, 'mlp_trainer_get_progress',                ctypes.c_double,            [ctypes.POINTER(MLPTrainer)])
        self.runTrainer                      = MLFunction(self, 'mlp_trainer_run',                         None,                       [ctypes.POINTER(MLPTrainer)])
        self.getTrainerError                 = MLFunction(self, 'mlp_trainer_error',                       ctypes.c_double,            [ctypes.POINTER(MLPTrainer)])
        self.saveTrainerProgress             = MLFunction(self, 'mlp_trainer_save_progression',            None,                       [ctypes.POINTER(MLPTrainer), ctypes.c_char_p])
        self.restoreTrainerProgress          = MLFunction(self, 'mlp_trainer_restore_progression',         None,                       [ctypes.POINTER(MLPTrainer), ctypes.c_char_p, ctypes.c_double, ctypes.c_double])
        self.getTrainerManagedNetwork        = MLFunction(self, 'mlp_trainer_get_network',                 ctypes.POINTER(MLPNetwork), [ctypes.POINTER(MLPTrainer)])
        self.newNetwork                      = MLFunction(self, 'mlp_network_new',                         ctypes.POINTER(MLPNetwork), [ctypes.c_char_p])
        self.deleteNetwork                   = MLFunction(self, 'mlp_network_delete',                      None,                       [ctypes.POINTER(MLPNetwork)])
        self.serializeNetwork                = MLFunction(self, 'mlp_network_serialize',                   None,                       [ctypes.POINTER(MLPNetwork), ctypes.c_char_p])
        self.deserializeNetwork              = MLFunction(self, 'mlp_network_deserialize',                 None,                       [ctypes.POINTER(MLPNetwork), ctypes.c_char_p])
        self.predict                         = MLFunction(self, 'mlp_network_predict',                     None,                       [ctypes.POINTER(MLPNetwork), ctypes.c_uint, ctypes.c_void_p])
        self.getNetworkOutput                = MLFunction(self, 'mlp_network_get_output',                  ctypes.c_void_p,            [ctypes.POINTER(MLPNetwork)])
        self.getNetworkOutputLength          = MLFunction(self, 'mlp_network_get_output_length',           ctypes.c_uint,              [ctypes.POINTER(MLPNetwork)])
        self.getNetworkLayerNumberOfNeuron   = MLFunction(self, 'mlp_network_get_layer_number_of_neuron',  ctypes.c_uint,              [ctypes.POINTER(MLPNetwork), ctypes.c_uint])
        self.getNetworkNumberOfLayer         = MLFunction(self, 'mlp_network_get_number_of_layer',         ctypes.c_uint,              [ctypes.POINTER(MLPNetwork)])
        self.getNetworkNumberOfInput         = MLFunction(self, 'mlp_network_get_number_of_input',         ctypes.c_uint,              [ctypes.POINTER(MLPNetwork)])
