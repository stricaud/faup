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

	$ echo "www.github.com" |faup -p
	scheme,credential,subdomain,domain,host,tld,port,resource_path,query_string,fragment
	,,www,github.com,www.github.com,com,,,,

	$ faup www.github.com
	,,www,github.com,www.github.com,com,,,,

Extract TLD > 1
---------------

We use the mozilla list that you can update like this:

      $ faup -u

Then the -t flag will search for TLDs against the mozilla list (and we output as json for clarity):

     $ faup -o json -t http://www.google.co.uk
     {
	"scheme": "http",
	"credential": "",
	"subdomain": "www",
	"domain": "google.co.uk",
	"host": "www.google.co.uk",
	"tld": "co.uk",
	"port": "",
	"resource_path": "",
	"query_string": "",
	"fragment": "",
     }

Extract only the TLD field
--------------------------

	$ faup -f tld slashdot.org
	org

	$ faup -f tld www.bbc.co.uk
	uk

	$ faup -f tld -t www.bbc.co.uk
	co.uk

Of course, without the -t flag we are faster because we are not checking against a list.


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

Building faup
-------------

To get and build faup, you need [cmake](http://www.cmake.org/). As cmake doesn't allow
to build the binary in the source directory, you have to create a build directory.

    git clone git://github.com/stricaud/faup.git
    cd faup
    mkdir build
    cd build
    cmake .. && make
