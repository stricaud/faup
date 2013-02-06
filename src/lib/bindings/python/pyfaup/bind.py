# -*- coding: utf-8 -*
# Copyright (C) Sebastien Tricaud 2012

from ctypes import *

def bind(cfunction, restype, *argtypes):
	func = getattr(bind.library, cfunction)
	func.restype = restype
	func.argtypes = argtypes
	return func

