Fast URL Library
================

The Fast URL Library provides:

* A static library you can embed in your software (furl_static)
* A dynamic library you can get along with (furll)
* A command line tool you can use to extract various parts of a url (furl)

Mission
-------

Furl is written to be fast and resilient to badly formated URL. It extract any 
required url field by checking each character once.

Why Yet Another URL Extraction Library?
---------------------------------------

Because they all suck. Find a library that can extract, say, a TLD even if you have 
an IP address, or http://localhost, or anything that may confuse your regex so much
that you end up with an unmaintainable one.

Architecture
------------

[ URL ] -> [ Features discovery ] -> [ Decoding ] -> [ URL Fields ]

Command line usage
------------------

Simply pipe or give your url as a parameter:

	$ echo "www.github.com" |furl
	scheme,credential,subdomain,domain,host,tld,port,resource_path,query_string,fragment
	,,www,github.com,www.github.com,com,,,,

	$ furl www.github.com
	scheme,credential,subdomain,domain,host,tld,port,resource_path,query_string,fragment
	,,www,github.com,www.github.com,com,,,,

Python bindings are also around. Here's what you can do:

       >>> from pyfurl.furl import Furl
       >>> f = Furl()
       >>> f.decode("https://www.slashdot.org")
       >>> f.get()
       {'credential': None, 'domain': 'slashdot.org', 'subdomain': 'www', 'fragment': None, 'host': 'www.slashdot.org', 'resource_path': None, 'tld': 'org', 'query_string': None, 'scheme': 'https', 'port': None}
       >>> 
