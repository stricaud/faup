#!/usr/bin/python

from pyfurl.furl import Furl

url = "http://www.wallinfire.net"

f = Furl()
print("We decode the url: %s" % (url))
f.decode(url)
data = f.get()
print("URL TLD: %s" % (data['tld']))

