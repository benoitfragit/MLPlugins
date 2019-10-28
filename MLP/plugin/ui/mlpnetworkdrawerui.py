#!/usr/bin/env python
# -*- coding: utf-8 -*-

from ui.network import MLNetworkDrawerBaseUI

class MLPNetworkDrawerUI(MLNetworkDrawerBaseUI):
    def __init__(self, parent = None):
        MLNetworkDrawerUI.__init__(self, parent)

        self._radius = 30
        self._sw     = 20
        self._sh     = 10

    def mlAddSignalRepresentation(self, *args, **kwargs):
        if len(args) >= 3 :
            N = args[0]         # Number of layers
            j = args[1]         # Signal index
            M = len(args[2])    # Number of input for this signal
            S = args[2]         # Signal

            xj = (2 * j + 1 - N) * (2 * self._radius + self._sw) / 2

            for i in range(M):
                yi = (2 * i + 1 - M) * (2 * self._radius + self._sh) / 2
                self._scene.addEllipse(xj, yi, self._radius, self._radius, self._pen)
