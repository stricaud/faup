"""Version and details for pyfaup"""

__description__ = "Python bindings for the faup library"
__url__ = "https://www.github.com/stricaud/faup"
__author__ = "Sebastien Tricaud"
__author_email__ = "sebastien@honeynet.org"
__license__ = "MIT"
__maintainer__ = __author__
__maintainer_email__ = __author_email__

# The package version is the faup library version (VERSION file at the repository
# root), injected into the wheel metadata at build time.
try:
    from importlib.metadata import version as _version
    __version__ = _version("pyfaup")
except Exception:  # pragma: no cover - not installed as a distribution
    __version__ = "unknown"
