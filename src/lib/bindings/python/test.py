#!/usr/bin/python

from pyfaup.faup import Faup
import re
import sys
import codecs
import binascii

# dynamically list all Faup's methods
methods = []
for m in dir( Faup ):
	if re.search("^get_", m) :
		methods.append( m )
methods.remove("get_version")


# run
# run
if len(sys.argv) != 2 :
    print "%s <file containing 1 url per line>" % sys.argv[0]
    sys.exit(0)

f = Faup()
file_urls=codecs.open(sys.argv[1],'r','ascii',errors='ignore')
urls=file_urls.readlines()

for url in urls:
    url=url.replace('\n','')
    print("URL:[%s]" % (url))
    f.decode(url)
#    print("-----> Extracted TLD:%s" % f.get_tld())
#    print("-----> Extracted TLD:%s" % f.get_domain_without_tld())

    for m in methods:
	fct = getattr(f, m)
	print "\t%s : %s" % (re.sub("^get_", "", m), fct())

