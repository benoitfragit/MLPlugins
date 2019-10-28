#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os

from mlploader import MLPLoader

from core  import MLPluginBase

from ui import MLPTrainerLoaderUI
from ui import MLPTrainerEditorUI

class MLPlugin(MLPluginBase, MLPLoader):
    def __init__(self):
        MLPluginBase.__init__(self)
        MLPLoader.__init__(self)

        self._trainerloaderui = MLPTrainerLoaderUI(self)
        self._trainereditorui = MLPTrainerEditorUI(self)

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
        ret = None

        if 'model' in trainer.keys():
            ret = self._funcs[self._funcnames.TRAINER_GET_MANAGED_NETWORK](trainer['model'])

        return ret

    """
    ....................................................................
    .......................... Plugin NETWORK api.......................
    ....................................................................
    """
    def mlGetNetwork(self, path):
        internal = self._funcs[self._funcnames.NETWORK_NEW](path)
        return internal

    def mlDeleteNetwork(self, network):
        self._funcs[self._funcnames.NETWORK_DELETE](network)

    def mlSaveNetwork(self, network, path):
        self._funcs[self._funcnames.NETWORK_SERIALIZE](network, path)

    def mlLoadNetwork(self, network, path):
        self._funcs[self._funcnames.NETWORK_DESERIALIZE](network, path)

    def mlPredict(self, network, num, sig):
        self._funcs[self._funcnames.NETWORK_PREDICT](network, num, sig)

    def mlGetNetworkOutputLength(self, network):
        return self._funcs[self._funcnames.NETWORK_GET_OUTPUT_LENGTH](network)

    def mlGetNetworkPrediction(self, network):
        return self._funcs[self._funcnames.NETWORK_GET_OUTPUT](network)

if __name__ == '__main__':
    l = MLPlugin()
