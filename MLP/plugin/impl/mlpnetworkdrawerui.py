#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt5.QtGui                        import QBrush
from PyQt5.QtGui                        import QColor
from ui.network.mlnetworkdrawerbaseui   import MLNetworkDrawerBaseUI
from PyQt5.QtWidgets                    import QGraphicsItem

class MLPNetworkDrawerUI(MLNetworkDrawerBaseUI):
    """

    """
    def __init__(self, radius = 80, sw = 300, sh = 20):
        MLNetworkDrawerBaseUI.__init__(self)

        self._radius = radius
        self._sw     = sw
        self._sh     = sh

        self._items    = {}

    def clear(self):
        """

        """
        MLNetworkDrawerBaseUI.clear(self)
        self._items.clear()

    def mlAddSignalRepresentation(self, ncols, j, M, title=''):
        """

        :param ncols:
        :param j:
        :param M:
        :param title:
        """
        # X coordinate for this signal
        xj = (2 * j + 1 - ncols) * (2 * self._radius + self._sw) / 2

        p = (2.0 * self._radius + self._sh) / 2.0
        o = (1 - M) * p

        # Display the text above the signal
        text = self.addSimpleText(title)
        text.setFlag(QGraphicsItem.ItemIgnoresTransformations)
        rect = text.boundingRect()
        text.setPos(xj, (1 - M) * (2 * self._radius + self._sh) / 2 - self._radius)

        # Display new signal
        self._items[j] = []
        for i in range(M):
            yi = 2 * i * p + o

            item = self.addEllipse(xj, yi, self._radius, self._radius, self._pen)

            if j > 0:
                for k in range(len(self._items[j - 1])):
                    x, y = self._items[j-1][k][0], self._items[j-1][k][1]
                    self.addLine(x + self._radius, y + 0.5 * self._radius, xj, yi + 0.5 * self._radius, self._pen)

            self._items[j].append([xj, yi, item, 0.0, 0.0, True])

    def mlOnUpdateSignalRepresentation(self, j, s):
        """

        :param j:
        :param s:
        """
        if len(s) > 0:
            for i in range(len(s)):
                if i < len(self._items[j]) and self._items[j][i] is not None:
                    if self._items[j][i][5]:
                        self._items[j][i][5] = False
                        self._items[j][i][3] = s[i]
                        self._items[j][i][4] = s[i]

                    if s[i] < self._items[j][i][3]:
                        self._items[j][i][3] = s[i]

                    if s[i] > self._items[j][i][4]:
                        self._items[j][i][4] = s[i]

                    # Color normalization
                    amplitude = abs(self._items[j][i][4] - self._items[j][i][3])

                    if amplitude > 0:
                        c =  255 * (s[i] - self._items[j][i][3]) / amplitude
                        brush = QBrush(QColor(c, c, c))
                        self._items[j][i][2].setBrush(brush)
