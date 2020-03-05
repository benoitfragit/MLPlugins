#!/usr/bin/env python
# -*- coding: utf-8 -*

import ctypes

class MLPData:
    """

    """
    def __init__(self, input_length, output_length, training_sample, evaluating_sample, labels_sample):
        self._input_length = input_length
        self._output_length = output_length
        self._training_sample = training_sample
        self._evaluating_sample = evaluating_sample
        self._labels_sample = labels_sample

    def __call__(self, *args, **kwargs):
        class Data(ctypes.Structure):
            _fields_ = [('training', self.mlDatasetFactory(self._training_sample)),
                        ('evaluating', self.mlDatasetFactory(self._evaluating_sample)),
                        ('input_length', ctypes.c_uint),
                        ('output_length', ctypes.c_uint),
                        ('labels_length', ctypes.c_uint),
                        ('labels', ctypes.POINTER(ctypes.c_char_p * self._labels_sample)),
                        ('is_labelled',ctypes.c_ubyte)]
        return Data

    def mlDatasetFactory(self, number_of_signal):
        """

        :param number_of_signal:
        :return:
        """
        class MLPDataset(ctypes.Structure):
            _fields_ = [('input',    ctypes.POINTER(ctypes.POINTER(ctypes.c_double * self._input_length)  * number_of_signal)),
                        ('output',   ctypes.POINTER(ctypes.POINTER(ctypes.c_double * self._output_length) * number_of_signal)),
                        ('children', ctypes.c_uint)]
        return MLPDataset