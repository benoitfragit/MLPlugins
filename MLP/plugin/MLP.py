#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os
import ctypes

from exchange  import MLPNetwork
from mlploader import MLPLoader

from core  import MLPluginBase

from ui import MLPTrainerLoaderUI
from ui import MLPTrainerEditorUI
from ui import MLPNetworkDrawerUI

class MLPModelManager:
    def __init__(self, internal, key = None):
        self._internal  = internal
        self._key       = None
        self._exception = None

    def __enter__(self):
        ret = None
        try:
            ret = self._internal[self._key]
        except Exception as e:
            self._exception = e
        return ret

    def __exit__(self, exc_type, exc_val, traceback):
        return True

class MLPlugin(MLPluginBase, MLPLoader):
    def __init__(self):
        MLPluginBase.__init__(self)
        MLPLoader.__init__(self)

        self.load()

        self._trainerloaderui = MLPTrainerLoaderUI(self)
        self._trainereditorui = MLPTrainerEditorUI(self)

    def load(self):
        # Call plugin init method
        if self.mlp_plugin_init is not None:
           self.mlp_plugin_init()
           self._activated = True

        # Get all metadata
        if  self.mlp_plugin_metadata is not None:
            meta = self.mlp_plugin_metadata().contents
            self._name      = meta.name
            self._version   = meta.version
            self._author    = meta.author
            self._description = meta.description
    """
    ....................................................................
    .......................... Plugin TRINER api........................
    ....................................................................
    """
    def mlGetTrainer(self, net, data):
        model = None
        if self.mlp_trainer_new is not None:
            model = self.mlp_trainer_new(net, data)
        return model

    def mlDeleteTrainer(self, trainer):
        with MLPModelManager(trainer, 'model') as model:
            if self.mlp_trainer_delete is not None:
                self.mlp_trainer_delete(trainer['model'])

    def mlConfigureTrainer(self, trainer, path):
        with MLPModelManager(trainer, 'model') as model:
            if self.mlp_trainer_configure is not None:
                self.mlp_trainer_configure(trainer['model'], path)
        with MLPModelManager(trainer, 'settings') as settings:
            trainer['settings'] = path

    def mlIsTrainerRunning(self, trainer):
        ret = False
        with MLPModelManager(trainer, 'model') as model:
            if self.mlp_trainer_is_running is not None:
                ret = self.mlp_trainer_is_running(trainer['model'])
        return ret

    def mlGetTrainerProgress(self, trainer):
        ret = 0.0
        with MLPModelManager(trainer, 'model') as model:
            if self.mlp_trainer_get_progress is not None:
                ret = self.mlp_trainer_get_progress(trainer['model'])
        return ret

    def mlTrainerRun(self, trainer):
        with MLPModelManager(trainer, 'model') as model:
            if self.mlp_trainer_run is not None:
                self.mlp_trainer_run(trainer['model'])

    def mlGetTrainerError(self, trainer):
        ret = 100.0
        with MLPModelManager(trainer, 'model') as model:
            if self.mlp_trainer_error is not None:
                ret = self.mlp_trainer_error(trainer['model'])
        return ret

    def mlSaveTrainerProgression(self, trainer, path):
        with MLPModelManager(trainer, 'model') as model:
            if self.mlp_trainer_save_progression is not None:
                real_path = path + '.xml'
                self.mlp_trainer_save_progression(trainer['model'], real_path)

    def mlRestoreTrainerProgression(self, trainer, path, progress, error):
        with MLPModelManager(trainer, 'model') as model:
            if self.mlp_trainer_restore_progression is not None:
                real_path =  path + '.xml'
                self.mlp_trainer_restore_progression(trainer['model'], real_path, progress, error)

    def mlGetLoadedTrainer(self):
        ret = None

        if self._trainerloaderui is not None:

            network_filepath    = self._trainerloaderui.mlGetNetworkFilePath()
            data_filepath       = self._trainerloaderui.mlGetDataFilePath()
            trainer_filepath    = self._trainerloaderui.mlGetTrainerFilePath()

            ret = self.mlGetTrainerInternal(network_filepath, data_filepath, trainer_filepath)

        return ret

    def mlTrainerJSONEncoding(self, trainer, d):
        with MLPModelManager(trainer, 'network') as network:
            d['network']  = trainer['network']
        with MLPModelManager(trainer, 'data') as data:
            d['data']     = trainer['data']
        with MLPModelManager(trainer, 'settings') as settings:
            d['settings'] = trainer['settings']

    def mlTrainerJSONDecoding(self, buf):
        ret = None
        if buf is not None:
            if  'network'   in buf.keys() and \
                'settings'  in buf.keys() and \
                'data'      in buf.keys():

                network_filepath = buf['network']
                trainer_filepath = buf['settings']
                data_filepath    = buf['data']

                ret = self.mlGetTrainerInternal(network_filepath, data_filepath, trainer_filepath)

        return ret

    def mlGetTrainerInternal(self,\
                             network_filepath, \
                             data_filepath, \
                             trainer_filepath):
        internal = None

        if network_filepath is not None      and \
            os.path.exists(network_filepath) and \
            os.path.isfile(network_filepath) and \
            data_filepath is not None        and \
            os.path.exists(data_filepath)    and \
            os.path.isfile(data_filepath):

            internal = {}

            internal['model']    = self.mlGetTrainer(network_filepath, data_filepath)

            internal['network']  = network_filepath
            internal['data']     = data_filepath
            internal['settings'] = trainer_filepath

            if  trainer_filepath is not None and \
                os.path.exists(trainer_filepath) and \
                os.path.isfile(trainer_filepath):
                self.mlConfigureTrainer(internal, trainer_filepath)

        return internal

    def mlTrainerGetManagedNetwork(self, trainer):
        internal = {}

        with MLPModelManager(trainer, 'model') as model:
            if self.mlp_trainer_get_network is not None:
                internal['model'] = self.mlp_trainer_get_network(trainer['model'])

        return internal

    """
    ....................................................................
    .......................... Plugin NETWORK api.......................
    ....................................................................
    """
    def mlGetNetwork(self, path):
        internal = {}
        if self.mlp_network_new is not None:
            internal['model'] = self.mlp_network_new(path)
        return internal

    def mlDeleteNetwork(self, network):
        with MLPModelManager(network, 'model') as model:
            if self.mlp_network_delete is not None:
                self.mlp_network_delete(network['model'])

    def mlSaveNetwork(self, network, path):
        with MLPModelManager(network, 'model') as model:
            if self.mlp_network_serialize is not None:
                self.mlp_network_serialize(network['model'], path)

    def mlLoadNetwork(self, network, path):
        with MLPModelManager(network, 'model') as model:
            if self.mlp_network_deserialize is not None:
                self.mlp_network_deserialize(network['model'], path)

    def mlPredict(self, network, num, sig):
        with MLPModelManager(network, 'model') as model:
            if self.mlp_network_predict is not None:
                self.mlp_network_predict(network['model'], num, sig)

    def mlGetNetworkOutputLength(self, network):
        ret = 0
        with MLPModelManager(network, 'model') as model:
            if self.mlp_network_get_output_length is not None:
                ret = self.mlp_network_get_output_length(network['model'])
        return ret

    def mlGetNetworkPrediction(self, network):
        ret = None
        with MLPModelManager(network, 'model') as model:
            if self.mlp_network_get_output is not None:
                ret = self.mlp_network_get_output(network['model'])
        return ret

    def mlGetNetworkNumberOfLayer(self, network):
        ret = 0
        with MLPModelManager(network, 'model') as model:
            if self.mlp_network_get_number_of_layer is not None:
                ret = self.mlp_network_get_number_of_layer(network['model'])
        return ret

    def mlGetNetworkNumberOfInput(self, network):
        ret = 0
        with MLPModelManager(network, 'model') as model:
            if self.mlp_network_get_number_of_input is not None:
                ret = self.mlp_network_get_number_of_input(network['model'])
        return ret

    def mlGetLayerNumberOfNeuron(self, network, i):
        ret = 0
        with MLPModelManager(network, 'model') as model:
            if self.mlp_network_get_layer_number_of_neuron is not None:
                ret = self.mlp_network_get_layer_number_of_neuron(network['model'], i)
        return ret

    def mlGetNetworkLayerOutputSignal(self, network, i):
        res = None

        with MLPModelManager(network, 'model') as model:
            numberOfNeuron = self.mlGetLayerNumberOfNeuron(network, i)

            if numberOfNeuron > 0:
                func = self.wrap('mlp_network_get_layer_output_signal', \
                                 ctypes.POINTER(ctypes.c_double * numberOfNeuron), \
                                 [ctypes.POINTER(MLPNetwork), ctypes.c_uint])
                res = func(network['model'], i).contents

        return res

    def mlGetNetworkInputSignal(self, network):
        res = None
        with MLPModelManager(network, 'model') as model:
            numberOfInput = self.mlGetNetworkNumberOfInput(network)

            if numberOfInput > 0:
                func = self.wrap('mlp_network_get_input_signal', \
                                ctypes.POINTER(ctypes.c_double*numberOfInput), \
                                [ctypes.POINTER(MLPNetwork)])
                res = func(network['model']).contents

        return res

    def mlGetNetworkDrawerUI(self):
        return MLPNetworkDrawerUI()

if __name__ == '__main__':
    l = MLPlugin()
