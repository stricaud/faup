#!/usr/bin/python

from pyfaup.faup import Faup

url = b"http://www.wallinfire.co.uk"

f = Faup()
print("We decode the url: %s" % (url))
f.decode(url, False)
data = f.get()
print(f.get_tld())
print("URL TLD: %s" % (data['tld']))

