#!/usr/bin/python

from pyfaup.faup import Faup

url = "http://www.wallinfire.net"

f = Faup()
print("We decode the url: %s" % (url))
f.decode(url)
data = f.get()
print("URL TLD: %s" % (data['tld']))

