# -*- coding: utf-8 -*
# Copyright (C) Sebastien Tricaud 2012-2020

import os
import sys
import platform

from ctypes import *

from pyfaup.bind import *

rundir = os.path.dirname(os.path.abspath(__file__))
os.environ["FAUP_DATA_DIR"] = rundir 

is_32bits = (sys.maxsize <= 2**32)
if is_32bits:
        raise ImportError("32 bits architectures not supported")

system = platform.system()

LOAD_LIB=""

if system == "Linux":
        LOAD_LIB=rundir + "/Linux/x86_64/libfaupl.so"
if system == "Darwin":
        LOAD_LIB=rundir + "/Darwin/x86_64/libfaupl.dylib"

#print(LOAD_LIB)
        
bind.library = cdll.LoadLibrary(LOAD_LIB)
