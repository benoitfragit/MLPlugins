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

class MLPlugin(MLPluginBase, MLPLoader):
    def __init__(self):
        MLPluginBase.__init__(self)
        MLPLoader.__init__(self)

        self.load()

        self._trainerloaderui = MLPTrainerLoaderUI(self)
        self._trainereditorui = MLPTrainerEditorUI(self)

    def load(self):
        # Call plugin init method
        if self.init is not None:
           self.init()
           self._activated = True

        # Get all metadata
        if  self.metadata is not None:
            meta = self.metadata().contents
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
        if self.newTrainer is not None:
            model = self.newTrainer(net, data)
        return model

    def mlDeleteTrainer(self, trainer):
        if trainer is not None and 'model' in trainer.keys() and self.deleteTrainer is not None:
            self.deleteTrainer(trainer['model'])

    def mlConfigureTrainer(self, trainer, path):
        if trainer is not None :
            if 'model' in trainer.keys() and self.configureTrainer is not None:
                self.configureTrainer(trainer['model'], path)
            if 'settings' in trainer.keys():
                trainer['settings'] = path

    def mlIsTrainerRunning(self, trainer):
        ret = False
        if trainer is not None and 'model' in trainer.keys() and self.isTrainerRunning is not None:
            ret = self.isTrainerRunning(trainer['model'])
        return ret

    def mlGetTrainerProgress(self, trainer):
        ret = 0.0
        if trainer is not None and 'model' in trainer.keys() and self.getTrainerProgress is not None:
            ret = self.getTrainerProgress(trainer['model'])
        return ret

    def mlTrainerRun(self, trainer):
        if trainer is not None and 'model' in trainer.keys() and self.runTrainer is not None:
            self.runTrainer(trainer['model'])

    def mlGetTrainerError(self, trainer):
        ret = 100.0
        if trainer is not None and 'model' in trainer.keys() and self.getTrainerError is not None:
            ret = self.getTrainerError(trainer['model'])
        return ret

    def mlSaveTrainerProgression(self, trainer, path):
        if trainer is not None and 'model' in trainer.keys() and self.saveTrainerProgress is not None:
            real_path = path + '.xml'
            self.saveTrainerProgress(trainer['model'], real_path)

    def mlRestoreTrainerProgression(self, trainer, path, progress, error):
        if trainer is not None and 'model' in trainer.keys() and self.restoreTrainerProgress is not None:
            real_path =  path + '.xml'
            self.restoreTrainerProgress(trainer['model'], real_path, progress, error)

    def mlGetLoadedTrainer(self):
        ret = None

        if self._trainerloaderui is not None:

            network_filepath    = self._trainerloaderui.mlGetNetworkFilePath()
            data_filepath       = self._trainerloaderui.mlGetDataFilePath()
            trainer_filepath    = self._trainerloaderui.mlGetTrainerFilePath()

            ret = self.mlGetTrainerInternal(network_filepath, data_filepath, trainer_filepath)

        return ret

    def mlTrainerJSONEncoding(self, trainer, d):
        if 'network' in trainer.keys():
            d['network']  = trainer['network']
        if 'data' in trainer.keys():
            d['data']     = trainer['data']
        if 'settings' in trainer.keys():
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

        if 'model' in trainer.keys() and self.getTrainerManagedNetwork is not None:
            internal['model'] = self.getTrainerManagedNetwork(trainer['model'])

        return internal

    """
    ....................................................................
    .......................... Plugin NETWORK api.......................
    ....................................................................
    """
    def mlGetNetwork(self, path):
        internal = {}
        if self.newNetwork is not None:
            internal['model'] = self.newNetwork(path)
        return internal

    def mlDeleteNetwork(self, network):
        if 'model' in network.keys() and self.deleteNetwork is not None:
            self.deleteNetwork(network['model'])

    def mlSaveNetwork(self, network, path):
        if 'model' in network.keys() and self.serializeNetwork is not None:
            self.serializeNetwork(network['model'], path)

    def mlLoadNetwork(self, network, path):
        if 'model' in network.keys() and self.deserializeNetwork is not None:
            self.deserializeNetwork(network['model'], path)

    def mlPredict(self, network, num, sig):
        if 'model' in network.keys() and self.predict is not None:
            self.predict(network['model'], num, sig)

    def mlGetNetworkOutputLength(self, network):
        ret = 0
        if 'model' in network.keys() and self.getNetworkOutputLength is not None:
            ret = self.getNetworkOutputLength(network['model'])
        return ret

    def mlGetNetworkPrediction(self, network):
        ret = None
        if 'model' in network.keys() and self.getNetworkOutput is not None:
            ret = self.getNetworkOutput(network['model'])
        return ret

    def mlGetNetworkNumberOfLayer(self, network):
        ret = 0
        if 'model' in network.keys() and self.getNetworkNumberOfLayer is not None:
            ret = self.getNetworkNumberOfLayer(network['model'])
        return ret

    def mlGetNetworkNumberOfInput(self, network):
        ret = 0
        if 'model' in network.keys() and self.getNetworkNumberOfInput is not None:
            ret = self.getNetworkNumberOfInput(network['model'])
        return ret

    def mlGetLayerNumberOfNeuron(self, network, i):
        ret = 0
        if 'model' in network.keys() and self.getNetworkLayerNumberOfNeuron is not None:
            ret = self.getNetworkLayerNumberOfNeuron(network['model'], i)
        return ret

    def mlGetNetworkLayerOutputSignal(self, network, i):
        res = None

        if 'model' in network.keys():
            numberOfNeuron = self.mlGetLayerNumberOfNeuron(network, i)

            if numberOfNeuron > 0:
                func = self.wrap('mlp_network_get_layer_output_signal', \
                                 ctypes.POINTER(ctypes.c_double * numberOfNeuron), \
                                 [ctypes.POINTER(MLPNetwork), ctypes.c_uint])
                res = func(network['model'], i).contents

        return res

    def mlGetNetworkInputSignal(self, network):
        res = None
        if 'model' in network.keys():
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
