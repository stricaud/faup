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
    #print("We decode the url: %s" % (url))
    #if sys.version.split('.')[0].split('.')[0]=='3':
    f.decode(bytes(url,'utf-8'), False)
    #if sys.version.split('.')[0].split('.')[0]=='2':
    #        f.decode(bytes(url),False)
    #data = f.get()
    f.get_tld()
    #f.get_domain()
    #f.get_subdomain()
    #print(f.get_tld())
    #print(f.get_domain())
    #print("URL TLD: %s" % (f.get_tld()))
