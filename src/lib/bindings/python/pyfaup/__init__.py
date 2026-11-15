# -*- coding: utf-8 -*
# Copyright (C) Sebastien Tricaud 2012-2026

import glob
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


def _find_library():
        """Locate libfaupl shipped inside the wheel (pyfaup/lib/), built by CMake
        through scikit-build-core for the current platform."""
        system = platform.system()
        if system == "Windows":
                patterns = ["faupl.dll", "libfaupl.dll"]
        elif system == "Darwin":
                patterns = ["libfaupl.*.dylib", "libfaupl.dylib"]
        else:
                patterns = ["libfaupl.so.*", "libfaupl.so"]

        libdir = os.path.join(rundir, "lib")
        for pattern in patterns:
                matches = sorted(glob.glob(os.path.join(libdir, pattern)))
                if matches:
                        return matches[0]

        # Fall back to a system-wide installation of faup.
        from ctypes.util import find_library
        found = find_library("faupl")
        if found:
                return found

        raise ImportError(
                "Cannot find the faup shared library (libfaupl) for %s/%s in %s"
                % (system, platform.machine(), libdir))


LOAD_LIB = _find_library()

bind.library = cdll.LoadLibrary(LOAD_LIB)
