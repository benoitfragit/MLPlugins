#!/usr/bin/env python
# -*- coding: utf-8 -*-

from ui.trainer.mltrainerloaderbaseui import MLTrainerLoaderBaseUI
from impl import data

from PyQt5.QtWidgets    import QLineEdit
from PyQt5.QtWidgets    import QLabel
from PyQt5.QtWidgets    import QPushButton
from PyQt5.QtWidgets    import QHBoxLayout
from PyQt5.QtWidgets    import QVBoxLayout
from PyQt5.QtWidgets    import QFileDialog
from PyQt5.QtWidgets    import QSizePolicy
from PyQt5.QtGui        import QIcon
from PyQt5.QtCore       import QSize
from PyQt5.QtCore       import Qt

from importlib          import resources

import os

class MLLoadButton(QPushButton):
    def __init__(self, text):
        QPushButton.__init__(self, text)
        self._text = text
        self.setFlat(True)
        self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        self.setStyleSheet("QPushButton {text-align:left;}")
        self.setLayoutDirection(Qt.RightToLeft)
        self._mainicon = None
        self._applyicon = None
        self._warningicon = None
        with resources.path(data, 'openfile.png') as p:
            self._mainicon = QIcon(str(p))
            self.setIcon(self._mainicon)
            self.setIconSize(QSize(100, 100))
        with resources.path(data, 'apply.png') as p:
            self._applyicon = QIcon(str(p))
        with resources.path(data, 'warning.png') as p:
            self._warningicon = QIcon(str(p))

    def setDefault(self):
        self.setText(self._text)
        self.setIcon(self._mainicon)

    def setApply(self):
        self.setIcon(self._applyicon)

    def setWarning(self):
        self.setText('File not found!')
        self.setIcon(self._warningicon)

class MLPTrainerLoaderUI(MLTrainerLoaderBaseUI):
    """

    """
    def __init__(self, plugin, parent = None):
        MLTrainerLoaderBaseUI.__init__(self, plugin, parent = None)

    def mlBuildTrainerLoaderMainWidget(self):
        """

        """
        label0 = QLabel('Choose the trainer name')

        self._entry = QLineEdit()
        
        self._button1 = MLLoadButton('Open a network settings file')
        self._button2 = MLLoadButton('Open a trainer settings file')
        self._button3 = MLLoadButton('Open a data file')
        
        cancel  = QPushButton('Cancel')
        cancel.setIcon(QIcon.fromTheme('edit-undo'))
        cancel.setFlat(True)
        validate= QPushButton('Apply')
        validate.setIcon(QIcon.fromTheme('system-run'))
        validate.setFlat(True)

        self._entry.textChanged.connect(self.mlOnTrainerNameChanged)
        self._button1.clicked.connect(self.mlOpenNetworkFile)
        self._button2.clicked.connect(self.mlOpenTrainerFile)
        self._button3.clicked.connect(self.mlOpenDataFile)
        cancel.clicked.connect(self.mlCancel)
        validate.clicked.connect(self.mlValidate)

        hbox0   = QHBoxLayout()
        hbox1   = QHBoxLayout()
        vbox    = QVBoxLayout()

        hbox0.addWidget(label0)
        hbox0.addStretch(1)
        hbox0.addWidget(self._entry)

        hbox1.addStretch(1)
        hbox1.addWidget(cancel)
        hbox1.addWidget(validate)

        vbox.addLayout(hbox0)
        vbox.addWidget(self._button1)
        vbox.addWidget(self._button2)
        vbox.addWidget(self._button3)
        vbox.addStretch(1)
        vbox.addLayout(hbox1)

        self._mainWidget.setLayout(vbox)

    def mlOnTrainerNameChanged(self, text):
        """

        :param text:
        """
        self._trainer_name = text

    def mlOpenNetworkFile(self):
        """

        """
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        self._network_filepath, _ = QFileDialog.getOpenFileName(self, "Select a network file", "", "All files (*);;Xml files (*.xml)", options=options)
        self._button1.setText(os.path.basename(self._network_filepath))
        if os.path.isfile(self._network_filepath):
            self._button1.setApply()
        else:
            self._button1.setWarning()

    def mlOpenTrainerFile(self):
        """

        """
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        self._trainer_filepath, _ = QFileDialog.getOpenFileName(self, "Select a network file", "", "All files (*);;Xml files (*.xml)", options=options)
        self._button2.setText(os.path.basename(self._trainer_filepath))
        if os.path.isfile(self._trainer_filepath):
            self._button2.setApply()
        else:
            self._button2.setWarning()

    def mlOpenDataFile(self):
        """

        """
        options = QFileDialog.Options()
        options |= QFileDialog.DontUseNativeDialog
        self._data_filepath, _ = QFileDialog.getOpenFileName(self, "Select a network file", "", "All files (*);;Xml files (*.xml)", options=options)
        self._button3.setText(os.path.basename(self._data_filepath))
        if os.path.isfile(self._data_filepath): 
            self._button3.setApply()
        else:
            self._button3.setWarning()

    def mlCancel(self):
        """

        """
        self.mlResetUI()
        self.close()

    def mlValidate(self):
        """

        """
        if self._trainer_name is not None and self._trainer_name != '' and  os.path.isfile(self._network_filepath) and os.path.isfile(self._trainer_filepath) and os.path.isfile(self._data_filepath):
            self.mlValidateTrainerSignal.emit()
        
        self.close()

    def mlResetUI(self):
        """

        """
        MLTrainerLoaderBaseUI.mlResetUI(self)
        self._entry.clear()
        self._button1.setDefault()
        self._button2.setDefault()
        self._button3.setDefault()
