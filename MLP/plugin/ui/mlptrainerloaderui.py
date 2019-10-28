#!/usr/bin/env python
# -*- coding: utf-8 -*-

from ui.trainer import MLTrainerLoaderBaseUI

from PyQt5.QtWidgets    import QLineEdit
from PyQt5.QtWidgets    import QLabel
from PyQt5.QtWidgets    import QPushButton
from PyQt5.QtWidgets    import QHBoxLayout
from PyQt5.QtWidgets    import QVBoxLayout
from PyQt5.QtWidgets    import QFileDialog
from PyQt5.QtGui        import QIcon

import os

class MLPTrainerLoaderUI(MLTrainerLoaderBaseUI):
    def __init__(self, plugin, parent = None):
        MLTrainerLoaderBaseUI.__init__(self, plugin, parent = None)

    def mlBuildTrainerLoaderMainWidget(self):
        label0 = QLabel('Choose the trainer name')
        label1 = QLabel('Open a network settings file')
        label2 = QLabel('Open a trainer settings file')
        label3 = QLabel('Open a data file')

        self._label4 = QLabel()
        self._label5 = QLabel()
        self._label6 = QLabel()
        self._label4.setVisible(False)
        self._label6.setVisible(False)
        self._label5.setVisible(False)

        self._entry = QLineEdit()
        button1 = QPushButton()
        button1.setFlat(True)
        button1.setIcon(QIcon.fromTheme('system-search'))
        button2 = QPushButton()
        button2.setFlat(True)
        button2.setIcon(QIcon.fromTheme('system-search'))
        button3 = QPushButton()
        button3.setFlat(True)
        button3.setIcon(QIcon.fromTheme('system-search'))
        cancel  = QPushButton('Cancel')
        cancel.setIcon(QIcon.fromTheme('edit-undo'))
        cancel.setFlat(True)
        validate= QPushButton('Apply')
        validate.setIcon(QIcon.fromTheme('system-run'))
        validate.setFlat(True)

        self._entry.textChanged.connect(self.mlOnTrainerNameChanged)
        button1.clicked.connect(self.mlOpenNetworkFile)
        button2.clicked.connect(self.mlOpenTrainerFile)
        button3.clicked.connect(self.mlOpenDataFile)
        cancel.clicked.connect(self.mlCancel)
        validate.clicked.connect(self.mlValidate)

        hbox0   = QHBoxLayout()
        hbox1   = QHBoxLayout()
        hbox2   = QHBoxLayout()
        hbox3   = QHBoxLayout()
        hbox4   = QHBoxLayout()
        vbox    = QVBoxLayout()

        hbox0.addWidget(label0)
        hbox0.addStretch(1)
        hbox0.addWidget(self._entry)
        hbox1.addWidget(label1)
        hbox1.addStretch(1)
        hbox1.addWidget(button1)
        hbox2.addWidget(label2)
        hbox2.addStretch(1)
        hbox2.addWidget(button2)
        hbox3.addWidget(label3)
        hbox3.addStretch(1)
        hbox3.addWidget(button3)
        hbox4.addStretch(1)
        hbox4.addWidget(cancel)
        hbox4.addWidget(validate)

        vbox.addLayout(hbox0)
        vbox.addLayout(hbox1)
        vbox.addWidget(self._label4)
        vbox.addLayout(hbox2)
        vbox.addWidget(self._label5)
        vbox.addLayout(hbox3)
        vbox.addWidget(self._label6)
        vbox.addStretch(1)
        vbox.addLayout(hbox4)

        self._mainWidget.setLayout(vbox)

    def mlOnTrainerNameChanged(self, text):
        self._trainer_name = text

    def mlOpenNetworkFile(self):
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        self._network_filepath, _ = QFileDialog.getOpenFileName(self, "Select a network file", "", "All files (*);;Xml files (*.xml)", options=options)
        self._label4.setText(os.path.basename(self._network_filepath))
        self._label4.setVisible(True)

    def mlOpenTrainerFile(self):
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        self._trainer_filepath, _ = QFileDialog.getOpenFileName(self, "Select a network file", "", "All files (*);;Xml files (*.xml)", options=options)
        self._label5.setText(os.path.basename(self._trainer_filepath))
        self._label5.setVisible(True)

    def mlOpenDataFile(self):
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        self._data_filepath, _ = QFileDialog.getOpenFileName(self, "Select a network file", "", "All files (*);;Xml files (*.xml)", options=options)
        self._label6.setText(os.path.basename(self._data_filepath))
        self._label6.setVisible(True)

    def mlCancel(self):
        self.mlResetUI()
        self.close()

    def mlValidate(self):
        self.mlValidateTrainerSignal.emit()
        self.close()

    def mlResetUI(self):
        MLTrainerLoaderBaseUI.mlResetUI(self)
        self._entry.clear()
        self._label4.setText('')
        self._label5.setText('')
        self._label6.setText('')
        self._label4.setVisible(False)
        self._label6.setVisible(False)
        self._label5.setVisible(False)
