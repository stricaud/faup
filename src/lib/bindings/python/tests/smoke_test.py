#!/usr/bin/env python3
"""Minimal end-to-end check used by cibuildwheel after building each wheel."""

import pyfaup
from pyfaup.faup import Faup

f = Faup()
print("faup library version:", f.get_version())
print("loaded library:", pyfaup.LOAD_LIB)

f.decode("https://www.github.com/stricaud/faup?q=1")
result = f.get()
print(result)

assert result["scheme"] == "https", result
assert result["tld"] == "com", result
assert result["domain"] == "github.com", result
assert result["subdomain"] == "www", result
assert result["resource_path"] == "/stricaud/faup", result
print("OK")
