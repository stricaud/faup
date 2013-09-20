<p align="center"><img src="doc/images/faup-logo.png"/></p>

**Faup** stands for Finally An Url Parser and is a library and command line tool to parse URLs and normalize fields with two constraints:
 1. Work with real-life urls (resilient to badly formated ones)
 2. Be fast: no allocation for string parsing and read characters only once


 * **Webpage**: [http://stricaud.github.io/faup/](http://stricaud.github.io/faup/)
 * **Source**: [https://github.com/stricaud/faup][github]
 * **Issues**: [https://github.com/stricaud/faup/issues][issues]
 * **Mailing List**: [libfaup@googlegroups.com](https://groups.google.com/d/forum/libfaup)

## Documentation

* [Command Line Tool][clidoc]
* [Library API documentation][libdoc]

## Quick Start

What is provided?
-----------------

* A static library you can embed in your software (faup_static)
* A dynamic library you can get along with (faupl)
* A command line tool you can use to extract various parts of a url (faup)

Why Yet Another URL Extraction Library?
---------------------------------------

Because they all suck. Find a library that can extract, say, a TLD even if you have 
an IP address, or http://localhost, or anything that may confuse your regex so much
that you end up with an unmaintainable one.

Command line usage
------------------

Simply pipe or give your url as a parameter:

	$ echo "www.github.com" |faup -p
	scheme,credential,subdomain,domain,host,tld,port,resource_path,query_string,fragment
	,,www,github.com,www.github.com,com,,,,

	$ faup www.github.com
	,,www,github.com,www.github.com,com,,,,

If that url is a file, multiple values will be unpacked:

   $ cat urls.txt 
   https://foo:bar@example.com
   localhost
   www.mozilla.org:80/index.php

   $ faup -p urls.txt 
   scheme,credential,subdomain,domain,domain_without_tld,host,tld,port,resource_path,query_string,fragment
   https,foo:bar,,example.com,example,example.com,com,,,,
   ,,,localhost,localhost,localhost,,,,,
   ,,www,mozilla.org,mozilla,www.mozilla.org,org,80,/index.php,,

Extract only the TLD field
--------------------------

Faup uses the [Mozilla list](http://mxr.mozilla.org/mozilla-central/source/netwerk/dns/effective_tld_names.dat?raw=1) to extract TLDs of level greater than one. Can handle exceptions, etc.

	$ faup -f tld slashdot.org
	org

	$ faup -f tld www.bbc.co.uk
	co.uk

Json output, high level TLDs
----------------------------

The Json output can be called like this:

	$ faup -o json www.takatoukiter.foobar.yokohama.jp
	{
		"scheme": "",
		"credential": "",
		"subdomain": "www",
		"domain": "takatoukiter.foobar.yokohama.jp",
		"domain_without_tld": "takatoukiter",
		"host": "www.takatoukiter.foobar.yokohama.jp",
		"tld": "foobar.yokohama.jp",
		"port": "",
		"resource_path": "",
		"query_string": "",
		"fragment": ""
	}


Building faup
-------------

To get and build faup, you need [cmake](http://www.cmake.org/). As cmake doesn't allow
to build the binary in the source directory, you have to create a build directory.

    git clone git://github.com/stricaud/faup.git
    cd faup
    mkdir build
    cd build
    cmake .. && make
    sudo make install


[github]: https://github.com/stricaud/faup
[issues]: https://github.com/stricaud/faup/issues
[libdoc]: doc/library.md
[clidoc]: doc/cli.md
