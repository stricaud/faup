#!/usr/bin/python

from pyfaup.faup import Faup
import sys
import codecs
import binascii

f = Faup()

def test_faup(urls_file):
    urls = urls_file.readlines()
    for url in urls:
        f.decode(bytes(url,'utf-8'), False)

if __name__ == "__main__":
    urls_file = codecs.open(sys.argv[1],'r','utf-8')
    test_faup(urls_file)
    urls_file.close()
