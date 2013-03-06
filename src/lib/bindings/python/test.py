#!/usr/bin/python

from pyfaup.faup import Faup
import sys
import codecs
import binascii

f = Faup()
file_urls=codecs.open(sys.argv[1],'r','ascii',errors='ignore')
urls=file_urls.readlines()
for url in urls:
    url=url.replace('\n','')
    print("URL:[%s]" % (url))
    f.decode(url, False)
    print("-----> Extracted TLD:%s" % f.get_tld())

