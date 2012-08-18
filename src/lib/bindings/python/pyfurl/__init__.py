# -*- coding: utf-8 -*
# Copyright (C) Sebastien Tricaud 2012

import sys
from ctypes import *

from bind import *

UNIX_LIBRARY_FILENAME = "libfurll.so.1"
WIN32_LIBRARY_FILENAME = "furll.dll"

if sys.platform == "win32":
	LIBRARY_FILENAME = WIN32_LIBRARY_FILENAME
else:
	LIBRARY_FILENAME = UNIX_LIBRARY_FILENAME
	bind.library = cdll.LoadLibrary(LIBRARY_FILENAME)
