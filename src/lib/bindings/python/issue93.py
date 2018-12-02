#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pprint
from pyfaup.faup import Faup

f = Faup()
f.decode("www.météo.fr")
pprint.pprint(f.get())

