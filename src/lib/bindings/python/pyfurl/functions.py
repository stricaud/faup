# -*- coding: utf-8 -*
# Copyright (C) Sebastien Tricaud 2012

from ctypes import *
from bind import *

furl_init = bind("furl_init", c_void_p)
furl_decode = bind("furl_decode", None, c_void_p, c_char_p)
furl_terminate = bind("furl_terminate", None, c_void_p)

