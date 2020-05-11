#!/bin/bash
python setup.py sdist bdist_wheel --universal
twine upload dist/* --repository-url https://upload.pypi.org/legacy/
