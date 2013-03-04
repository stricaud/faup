#!/usr/bin/python

from pyfaup.faup import Faup
import sys
f = Faup()
file_urls=open(sys.argv[1],'r')
urls=file_urls.readlines()
for url in urls:
    print("We decode the url: %s" % (url))
    f.decode(url, False)
    data = f.get()
    print(f.get_tld())
    print(f.get_host())
    print("URL TLD: %s" % (data['tld']))
