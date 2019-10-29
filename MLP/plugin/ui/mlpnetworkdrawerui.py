#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt5.QtGui     import QBrush
from PyQt5.QtGui     import QColor
from ui.network      import MLNetworkDrawerBaseUI

class MLPNetworkDrawerUI(MLNetworkDrawerBaseUI):
    def __init__(self, plugin):
        MLNetworkDrawerBaseUI.__init__(self)

        self._plugin = plugin
        self._radius = 100
        self._sw     = 300
        self._sh     = 50

        self._items   = {}

    def mlAddSignalRepresentation(self, *args, **kwargs):
        if len(args) >= 4 :
            N = args[0]         # Number of layers
            j = args[1]         # Signal index
            M = len(args[2])    # Number of input for this signal
            S = args[2]         # Signal
            title = args[3]     # title

            # X coordinate for this signal
            xj = (2 * j + 1 - N) * (2 * self._radius + self._sw) / 2

            # Color normalization
            maxS = max(S)
            minS = min(S)

            amplitude = abs(minS - maxS)

            # Display the text above the signal
            text = self.addSimpleText(title)
            rect = text.boundingRect()
            text.setPos(xj, (1 - M) * (2 * self._radius + self._sh) / 2 - self._radius)

            # Display new signal
            self._items[j] = []
            for i in range(M):
                brush = QBrush()

                if amplitude > 0:
                    c =  255 * (maxS - S[1]) / (maxS - minS)
                    brush = QBrush(QColor(c, c, c))

                yi = (2 * i + 1 - M) * (2 * self._radius + self._sh) / 2

                item = self.addEllipse(xj, yi, self._radius, self._radius, self._pen, brush)

                if j > 0:
                    for k in range(len(self._items[j - 1])):
                        x, y = self._items[j-1][k][0], self._items[j-1][k][1]
                        self.addLine(x + self._radius, y + 0.5 * self._radius, xj, yi + 0.5*self._radius, self._pen)

                self._items[j].append((xj, yi, item))

    def mlOnDisplayNetwork(self, *args, **kwargs):
        if len(args) >= 1:
            internal = args[0]

            # Grab some data
            numberOfLayers = self._plugin.mlGetNetworkNumberOfLayer(internal)
            numberOfInputs = self._plugin.mlGetNetworkNumberOfInput(internal)

            # Add network input signal representation
            self.mlAddSignalRepresentation(numberOfLayers + 1, 0, [0] * numberOfInputs, "in")

            for i in range(numberOfLayers):
                # Grab some information about this layer
                numberOfNeurons = self._plugin.mlGetLayerNumberOfNeuron(internal, i)

                # Add layer representation
                self.mlAddSignalRepresentation(numberOfLayers + 1, i + 1, [0] * numberOfNeurons, "Layer " + str(i))
