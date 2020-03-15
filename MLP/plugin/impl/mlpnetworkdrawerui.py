#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt5.QtGui                        import QBrush
from PyQt5.QtGui                        import QColor
from ui.network.mlnetworkdrawerbaseui   import MLNetworkDrawerBaseUI
from PyQt5.QtWidgets                    import QGraphicsItem
from PyQt5.QtWidgets                    import QGraphicsEllipseItem
from PyQt5.QtCore                       import pyqtSignal
from PyQt5.QtCore                       import Qt

import uuid

class MLCircleValue(QGraphicsEllipseItem):
    def __init__(self, x, y, w, h):
        QGraphicsEllipseItem.__init__(self,x, y, w, h)

        self._creation = True
        self._min = 0.0
        self._max = 0.0 
        self._x   = x
        self._y   = y
        self._value  = 0.0
        self._focused = False

    def mouseDoubleClickEvent(self, event):
        self._focused = not self._focused
        if self._focused:
            self.scene().setSceneRect(self.sceneBoundingRect())
        else:
            self.scene().setSceneRect(self.scene().itemsBoundingRect())

        if len(self.scene().views()) > 0:
            view = self.scene().views()[0]
            view.fitInView(self.scene().sceneRect(), Qt.KeepAspectRatio)

        QGraphicsEllipseItem.mouseDoubleClickEvent(self, event)

    @property
    def value(self):
        return self._value

    @property
    def x(self):
        return self._x

    @property
    def y(self):
        return self._y

    @value.setter
    def value(self, val):
        if self._creation or val < self._min:
            self._min = val

        if self._creation or val > self._max:
            self._max = val
            
        self._creation = False
        self._value = val

        amplitude = self._max - self._min

        if amplitude > 0:
            c =  255 * (self._value - self._min) / amplitude
            brush = QBrush(QColor(c, c, c))
            self.setBrush(brush)

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
        text.setPos(xj + (self._radius - rect.width())/2, (1 - M) * (2 * self._radius + self._sh) / 2 - self._radius)
        
        # Display new signal
        self._items[j] = []
        for i in range(M):
            yi = 2 * i * p + o
            
            circle = MLCircleValue(xj, yi, self._radius, self._radius)
            circle.setPen(self._pen)
            
            if j > 0:
                circle.setFlag(QGraphicsItem.ItemIsSelectable)
                for k in range(len(self._items[j - 1])):
                    x, y = self._items[j-1][k].x, self._items[j-1][k].y
                    self.addLine(x + self._radius, y + 0.5 * self._radius, xj, yi + 0.5 * self._radius, self._pen)

            self.addItem(circle)
            self._items[j].append(circle)

    def mlOnUpdateSignalRepresentation(self, j, s):
        """

        :param j:
        :param s:
        """
        if len(s) > 0:
            for i in range(len(s)):
                if i < len(self._items[j]) and self._items[j][i] is not None:
                    self._items[j][i].value = s[i]
                    
