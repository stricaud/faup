#!/usr/bin/python
from setuptools import setup, find_packages
#from distutils.core import setup
import os
import textwrap

current_dir = os.path.abspath(os.path.dirname(__file__))

about = {}
with open(os.path.join(current_dir, "pyfaup", "__version__.py"), "r") as fp:
          exec(fp.read(), about)

setup(
    name='pyfaup',
    version = about["__version__"],
    description= about["__description__"],
    author= about["__author__"],
    author_email= about["__author_email__"],
    long_description=textwrap.dedent(open("README.rst", "r").read()),
    long_description_content_type="text/x-rst",
    maintainer=about["__maintainer__"],
    maintainer_email=about["__maintainer_email__"],
    url=about["__url__"],
    license=about["__license__"],
    packages = ["pyfaup"],
    package_dir = {"pyfaup": "pyfaup/"},
    package_data = {'pyfaup': ['Linux/x86_64/libfaupl.so','Darwin/x86_64/libfaupl.dylib','mozilla.tlds']},
)
