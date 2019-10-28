#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt5.QtGui     import QBrush
from PyQt5.QtGui     import QColor
from ui.network      import MLNetworkDrawerBaseUI

class MLPNetworkDrawerUI(MLNetworkDrawerBaseUI):
    def __init__(self, parent = None):
        MLNetworkDrawerBaseUI.__init__(self)

        self._radius = 60
        self._sw     = 40
        self._sh     = 20

    def mlAddSignalRepresentation(self, *args, **kwargs):
        if len(args) >= 3 :
            N = args[0]         # Number of layers
            j = args[1]         # Signal index
            M = len(args[2])    # Number of input for this signal
            S = args[2]         # Signal

            xj = (2 * j + 1 - N) * (2 * self._radius + self._sw) / 2

            maxS = max(S)
            minS = min(S)

            for i in range(M):
                c =  255 * (maxS - S[1]) / (maxS - minS)
                brush = QBrush(QColor(c, c, c))
                yi = (2 * i + 1 - M) * (2 * self._radius + self._sh) / 2
                self.addEllipse(xj, yi, self._radius, self._radius, self._pen, brush)
