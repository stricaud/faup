#!/usr/bin/python
import sys
import re

from urllib.parse import urlparse

def test_urllib(urls_file):
    for line in urls_file.readlines():
        o = urlparse(line)


if __name__ == "__main__":
    urls_file = open(sys.argv[1], "r")
    test_urllib(urls_file)
    urls_file.close()
