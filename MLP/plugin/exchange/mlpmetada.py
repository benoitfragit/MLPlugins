#!/usr/bin/env python
# -*- coding: utf-8 -*

import ctypes

class MLPMetaData(ctypes.Structure):
    _fields_ = [('name',        ctypes.c_char_p),
                ('author',      ctypes.c_char_p),
                ('version',     ctypes.c_char_p),
                ('description', ctypes.c_char_p)]
