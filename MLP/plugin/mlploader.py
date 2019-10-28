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

class MLPLoader(MLLoader):
    def __init__(self):
        MLLoader.__init__(self, 'libMLP.so')

        self._funcnames = enum(   'INIT',
                                  'METADATA',
                                  'TRAINER_NEW',
                                  'TRAINER_DELETE',
                                  'TRAINER_CONFIGURE',
                                  'TRAINER_IS_RUNNING',
                                  'TRAINER_GET_PROGRESS',
                                  'TRAINER_RUN',
                                  'TRAINER_ERROR',
                                  'TRAINER_SAVE_PROGRESSION',
                                  'TRAINER_RESTORE_PROGRESSION',
                                  'TRAINER_GET_MANAGED_NETWORK',
                                  'NETWORK_NEW',
                                  'NETWORK_DELETE',
                                  'NETWORK_SERIALIZE',
                                  'NETWORK_DESERIALIZE',
                                  'NETWORK_PREDICT',
                                  'NETWORK_GET_OUTPUT',
                                  'NETWORK_GET_OUTPUT_LENGTH')

        self._number_of_functions = (self._funcnames.NETWORK_GET_OUTPUT_LENGTH - self._funcnames.INIT + 1)

        self._api = [   ['mlp_plugin_init',                 None,                           []],
                        ['mlp_plugin_metadata',             ctypes.POINTER(MLPMetaData),    []],
                        ['mlp_trainer_new',                 ctypes.POINTER(MLPTrainer),     [ctypes.c_char_p, ctypes.c_char_p]],
                        ['mlp_trainer_delete',              None,                           [ctypes.POINTER(MLPTrainer)]],
                        ['mlp_trainer_configure',           None,                           [ctypes.POINTER(MLPTrainer), ctypes.c_char_p]],
                        ['mlp_trainer_is_running',          ctypes.c_ubyte,                 [ctypes.POINTER(MLPTrainer)]],
                        ['mlp_trainer_get_progress',        ctypes.c_double,                [ctypes.POINTER(MLPTrainer)]],
                        ['mlp_trainer_run',                 None,                           [ctypes.POINTER(MLPTrainer)]],
                        ['mlp_trainer_error',               ctypes.c_double,                [ctypes.POINTER(MLPTrainer)]],
                        ['mlp_trainer_save_progression',    None,                           [ctypes.POINTER(MLPTrainer), ctypes.c_char_p]],
                        ['mlp_trainer_restore_progression', None,                           [ctypes.POINTER(MLPTrainer), ctypes.c_char_p, ctypes.c_double, ctypes.c_double]],
                        ['mlp_trainer_get_network',         ctypes.POINTER(MLPNetwork),     [ctypes.POINTER(MLPTrainer)]],
                        ['mlp_network_new',                 ctypes.POINTER(MLPNetwork),     [ctypes.c_char_p]],
                        ['mlp_network_delete',              None,                           [ctypes.POINTER(MLPNetwork)]],
                        ['mlp_network_serialize',           None,                           [ctypes.POINTER(MLPNetwork), ctypes.c_char_p]],
                        ['mlp_network_deserialize',         None,                           [ctypes.POINTER(MLPNetwork), ctypes.c_char_p]],
                        ['mlp_network_predict',             None,                           [ctypes.POINTER(MLPNetwork), ctypes.c_uint, ctypes.c_void_p]],
                        ['mlp_network_get_output',          ctypes.c_void_p,                [ctypes.POINTER(MLPNetwork)]],
                        ['mlp_network_get_output_length',   ctypes.c_uint,                  [ctypes.POINTER(MLPNetwork)]]]

        self._funcs = {}

        self.load()

    def load(self):
        for i in range(self._number_of_functions):
            self._funcs[i] = self.wrap(self._api[i][0],
                                        self._api[i][1],
                                        self._api[i][2])

            if self._funcs[i] is None:
                print >> sys.stderr, 'Method:' + self._api[i][0] + ' hasn t been loaded'
