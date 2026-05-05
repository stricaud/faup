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
arch = platform.machine()

LOAD_LIB="libfaupl.so"

if system == "Linux":
        LOAD_LIB=rundir + "/Linux/x86_64/libfaupl.so"
if system == "Darwin":
        if arch == "arm64":
                LOAD_LIB=rundir + "/Darwin/arm64/libfaupl.dylib"
        else:
                LOAD_LIB=rundir + "/Darwin/x86_64/libfaupl.dylib"

#print(LOAD_LIB)

try:
        bind.library = cdll.LoadLibrary(LOAD_LIB)
except OSError:
        raise ImportError("Could not find faup system library, please install it with your package manager")
