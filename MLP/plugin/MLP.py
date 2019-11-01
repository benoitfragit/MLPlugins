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

        self._trainerloaderui = MLPTrainerLoaderUI(self)
        self._trainereditorui = MLPTrainerEditorUI(self)
        self._networkdrawerui = MLPNetworkDrawerUI()

    def load(self):
        MLPLoader.load(self)

        # Call plugin init method
        if self._funcs[self._funcnames.INIT] is not None:
           self._funcs[self._funcnames.INIT]()
           self._activated = True

        # Get all metadata
        if  self._funcs[self._funcnames.METADATA] is not None:
            metadata = self._funcs[self._funcnames.METADATA]().contents
            self._name      = metadata.name
            self._version   = metadata.version
            self._author    = metadata.author
            self._description = metadata.description

    """
    ....................................................................
    .......................... Plugin TRINER api........................
    ....................................................................
    """
    def mlGetTrainer(self, net, data):
        model = self._funcs[self._funcnames.TRAINER_NEW](net, data)
        return model

    def mlDeleteTrainer(self, trainer):
        if trainer is not None and 'model' in trainer.keys():
            self._funcs[self._funcnames.TRAINER_DELETE](trainer['model'])

    def mlConfigureTrainer(self, trainer, path):
        if trainer is not None :
            if 'model' in trainer.keys():
                self._funcs[self._funcnames.TRAINER_CONFIGURE](trainer['model'], path)
            if 'settings' in trainer.keys():
                trainer['settings'] = path

    def mlIsTrainerRunning(self, trainer):
        ret = False
        if trainer is not None and 'model' in trainer.keys():
            ret = self._funcs[self._funcnames.TRAINER_IS_RUNNING](trainer['model'])
        return ret

    def mlGetTrainerProgress(self, trainer):
        ret = 0.0
        if trainer is not None and 'model' in trainer.keys():
            ret = self._funcs[self._funcnames.TRAINER_GET_PROGRESS](trainer['model'])
        return ret

    def mlTrainerRun(self, trainer):
        if trainer is not None and 'model' in trainer.keys():
            self._funcs[self._funcnames.TRAINER_RUN](trainer['model'])

    def mlGetTrainerError(self, trainer):
        ret = 100.0
        if trainer is not None and 'model' in trainer.keys():
            ret = self._funcs[self._funcnames.TRAINER_ERROR](trainer['model'])
        return ret

    def mlSaveTrainerProgression(self, trainer, path):
        if trainer is not None and 'model' in trainer.keys():
            real_path = path + '.xml'
            self._funcs[self._funcnames.TRAINER_SAVE_PROGRESSION](trainer['model'], real_path)

    def mlRestoreTrainerProgression(self, trainer, path, progress, error):
        if trainer is not None and 'model' in trainer.keys():
            real_path =  path + '.xml'
            self._funcs[self._funcnames.TRAINER_RESTORE_PROGRESSION](trainer['model'], real_path, progress, error)

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

        if 'model' in trainer.keys():
            internal['model'] = self._funcs[self._funcnames.TRAINER_GET_MANAGED_NETWORK](trainer['model'])

        return internal

    """
    ....................................................................
    .......................... Plugin NETWORK api.......................
    ....................................................................
    """
    def mlGetNetwork(self, path):
        internal = {}
        internal['model'] = self._funcs[self._funcnames.NETWORK_NEW](path)
        return internal

    def mlDeleteNetwork(self, network):
        if 'model' in network.keys():
            self._funcs[self._funcnames.NETWORK_DELETE](network['model'])

    def mlSaveNetwork(self, network, path):
        if 'model' in network.keys():
            self._funcs[self._funcnames.NETWORK_SERIALIZE](network['model'], path)

    def mlLoadNetwork(self, network, path):
        if 'model' in network.keys():
            self._funcs[self._funcnames.NETWORK_DESERIALIZE](network['model'], path)

    def mlPredict(self, network, num, sig):
        if 'model' in network.keys():
            self._funcs[self._funcnames.NETWORK_PREDICT](network['model'], num, sig)

    def mlGetNetworkOutputLength(self, network):
        ret = 0
        if 'model' in network.keys():
            ret = self._funcs[self._funcnames.NETWORK_GET_OUTPUT_LENGTH](network['model'])
        return ret

    def mlGetNetworkPrediction(self, network):
        ret = None
        if 'model' in network.keys():
            ret = self._funcs[self._funcnames.NETWORK_GET_OUTPUT](network['model'])
        return ret

    def mlGetNetworkNumberOfLayer(self, network):
        ret = 0
        if 'model' in network.keys():
            ret = self._funcs[self._funcnames.NETWORK_GET_NUMBER_OF_LAYER](network['model'])
        return ret

    def mlGetNetworkNumberOfInput(self, network):
        ret = 0
        if 'model' in network.keys():
            ret = self._funcs[self._funcnames.NETWORK_GET_NUMBER_OF_INPUT](network['model'])
        return ret

    def mlGetLayerNumberOfNeuron(self, network, i):
        ret = 0
        if 'model' in network.keys():
            ret = self._funcs[self._funcnames.NETWORK_LAYER_GET_NUMBER_OF_NEURON](network['model'], i)
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

if __name__ == '__main__':
    l = MLPlugin()
