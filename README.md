Faup: Finally an url parser! Library and Tools
==============================================

The Faup Library provides:

* A static library you can embed in your software (faup_static)
* A dynamic library you can get along with (faupl)
* A command line tool you can use to extract various parts of a url (faup)

Mission
-------

Faup is written to be fast and resilient to badly formated URL. It extract any 
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

	$ echo "www.github.com" |faup
	scheme,credential,subdomain,domain,host,tld,port,resource_path,query_string,fragment
	,,www,github.com,www.github.com,com,,,,

	$ faup www.github.com
	scheme,credential,subdomain,domain,host,tld,port,resource_path,query_string,fragment
	,,www,github.com,www.github.com,com,,,,

Python bindings
---------------

Here's what you can do:

       >>> from pyfaup.faup import Faup
       >>> f = Faup()
       >>> f.decode("https://www.slashdot.org")
       >>> f.get()
       {'credential': None, 'domain': 'slashdot.org', 'subdomain': 'www', 'fragment': None, 'host': 'www.slashdot.org', 'resource_path': None, 'tld': 'org', 'query_string': None, 'scheme': 'https', 'port': None}
       >>> 

C API
-----

Again, things are basic:

       faup_handler_t *fh;

       fh = faup_init();
       faup_decode(fh, "https://wallinfire.net", strlen("https://wallinfire.net"));
       tld_pos = faup_get_tld_pos(fh); /* will return 19 */       
       tld_size = faup_get_tld_size(fh); /* will return 3 */       
       faup_show(fh, ',', stdout);

       faup_terminate(fh);