#!/usr/bin/python
import sys

from PySide.QtCore import QUrl


def test_qurl(urls_file):
    for line in urls_file.readlines():
        o = QUrl(line)


if __name__ == "__main__":
    urls_file = open(sys.argv[1], "r")
    test_qurl(urls_file)
    urls_file.close()
