#!/usr/bin/env python
# -*- coding: utf-8 -*-

from ui.trainer import MLTrainerEditorBaseUI

from PyQt5.QtWidgets import QComboBox
from PyQt5.QtWidgets import QLabel
from PyQt5.QtWidgets import QDoubleSpinBox
from PyQt5.QtWidgets import QSpinBox
from PyQt5.QtWidgets import QHBoxLayout
from PyQt5.QtWidgets import QVBoxLayout
from PyQt5.QtWidgets import QPushButton

from PyQt5.QtGui     import QIcon

from PyQt5.QtCore    import Qt

import os
import xml.etree.ElementTree as ET

class MLPTrainerEditorUI(MLTrainerEditorBaseUI):
    def __init__(self, plugin, parent = None):
        self._trainer = None

        self._costfunction  = QComboBox()
        self._costfunction.addItems(["CrossEntropy", "Quadratic"])
        self._costfunction.setToolTip('This define how the error is computed')

        self._error         = QDoubleSpinBox()
        self._error.setRange(0.0, 100.0)
        self._error.setDecimals(4)
        self._error.setSuffix('%')
        self._error.setAlignment(Qt.AlignRight)
        self._error.setFrame(False)
        self._error.setToolTip('Define the error threshold to stop the training')

        self._iterations    = QSpinBox()
        self._iterations.setRange(0, 1000000)
        self._iterations.setAlignment(Qt.AlignRight)
        self._iterations.setFrame(False)
        self._iterations.setToolTip('Define the maximum number of iteration')

        self._minibatch     = QSpinBox()
        self._minibatch.setRange(1, 1000)
        self._minibatch.setAlignment(Qt.AlignRight)
        self._minibatch.setFrame(False)
        self._minibatch.setToolTip('Define number of sample in all minibatch sample')

        self._learning      = QDoubleSpinBox()
        self._learning.setRange(0.0, 5.0)
        self._learning.setAlignment(Qt.AlignRight)
        self._learning.setFrame(False)
        self._learning.setToolTip('Define the learning rate value, if it low learning will be slow and it could stuck into a local minimum else learning will be faster but may not find the minimum')

        self._momemtum      = QDoubleSpinBox()
        self._momemtum.setRange(0.0, 5.0)
        self._momemtum.setAlignment(Qt.AlignRight)
        self._momemtum.setFrame(False)
        self._momemtum.setToolTip('Define the inertial parameter value for the training')

        self._cancel  = QPushButton('Cancel')
        self._cancel.setIcon(QIcon.fromTheme('edit-undo'))
        self._cancel.setFlat(True)
        self._cancel.clicked.connect(self.mlCancel)

        self._validate= QPushButton('Apply')
        self._validate.setIcon(QIcon.fromTheme('system-run'))
        self._validate.setFlat(True)
        self._validate.clicked.connect(self.mlValidate)

        MLTrainerEditorBaseUI.__init__(self, plugin, parent)

    def mlBuildTrainerEditorMainWidget(self):
        label1       = QLabel('Cost-Function')
        label2       = QLabel('Error')
        label3       = QLabel('Max-iterations')
        label4       = QLabel('Mini-Batch-Size')
        label5       = QLabel('Learning-Rate')
        label6       = QLabel('Momemtum')

        hbox1        = QHBoxLayout()
        hbox2        = QHBoxLayout()
        hbox3        = QHBoxLayout()
        hbox4        = QHBoxLayout()
        hbox5        = QHBoxLayout()
        hbox6        = QHBoxLayout()
        hbox7        = QHBoxLayout()

        hbox1.addWidget(label1)
        hbox1.addStretch(1)
        hbox1.addWidget(self._costfunction)

        hbox2.addWidget(label2)
        hbox2.addStretch(1)
        hbox2.addWidget(self._error)

        hbox3.addWidget(label3)
        hbox3.addStretch(1)
        hbox3.addWidget(self._iterations)

        hbox4.addWidget(label4)
        hbox4.addStretch(1)
        hbox4.addWidget(self._minibatch)

        hbox5.addWidget(label5)
        hbox5.addStretch(1)
        hbox5.addWidget(self._learning)

        hbox6.addWidget(label6)
        hbox6.addStretch(1)
        hbox6.addWidget(self._momemtum)

        hbox7.addStretch(1)
        hbox7.addWidget(self._cancel)
        hbox7.addWidget(self._validate)

        vbox = QVBoxLayout()

        vbox.addLayout(hbox1)
        vbox.addLayout(hbox2)
        vbox.addLayout(hbox3)
        vbox.addLayout(hbox4)
        vbox.addLayout(hbox5)
        vbox.addLayout(hbox6)
        vbox.addStretch(1)
        vbox.addLayout(hbox7)

        self._mainWidget.setLayout(vbox)

    def mlResetUI(self):
        MLTrainerEditorBaseUI.mlResetUI(self)
        self._trainer = None

    def fromTrainer(self, *args, **kwargs):
        if args[0] is not None:
            self.setVisible(True)

            self._trainer = args[0]
            path = self._trainer['settings']

            if os.path.exists(path) and os.path.isfile(path):
                tree = ET.parse(path)
                root = tree.getroot()

                if root.tag == 'backpropagation':
                    learning        = float (root.attrib['learning-rate'])
                    minibatch       = int   (root.attrib['mini-batch-size'])
                    momemtum        = float (root.attrib['momentum'])
                    iterations      = int   (root.attrib['iterations'])
                    error           = float (root.attrib['error'])
                    costfunction    =        root.attrib['cost-function']

                    self._error.setValue(error)
                    self._iterations.setValue(iterations)
                    self._minibatch.setValue(minibatch)
                    self._learning.setValue(learning)
                    self._momemtum.setValue(momemtum)
                    idx = self._costfunction.findText(costfunction, Qt.MatchFixedString)
                    if idx >= 0:
                        self._costfunction.setCurrentIndex(idx)

    def mlCancel(self):
        self.mlResetUI()
        self.close()

    def mlValidate(self):
        if self._trainer is not None:
            backpropagation = ET.Element('backpropagation')

            # save the xml file
            backpropagation.set('learning-rate'  , str(self._learning.value()))
            backpropagation.set('mini-batch-size', str(self._minibatch.value()))
            backpropagation.set('momentum'       , str(self._momemtum.value()))
            backpropagation.set('iterations'     , str(self._iterations.value()))
            backpropagation.set('error'          , str(self._error.value()))
            backpropagation.set('cost-function'  , str(self._costfunction.currentText()))

            data = ET.tostring(backpropagation)
            with open(self._trainer['settings'], 'w') as settings:
                settings.write(data)
                settings.close()

            # Reload the xml file
            self._plugin.mlConfigureTrainer(self._trainer, self._trainer['settings'])

        self.close()
