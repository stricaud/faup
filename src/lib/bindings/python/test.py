#!/usr/bin/python

from pyfaup.faup import Faup
import sys
import codecs
import binascii

f = Faup()
file_urls=codecs.open(sys.argv[1],'r','utf-8')
urls=file_urls.readlines()
for url in urls:
    print("We decode the url: %s" % (url))
    if sys.version.split('.')[0].split('.')[0]=='3':
        f.decode(bytes(url,'utf-8'), False)
    if sys.version.split('.')[0].split('.')[0]=='2':
        f.decode(url, False)
    data = f.get()
    print(f.get_tld())
    print(f.get_host())
    print("URL TLD: %s" % (data['tld']))
