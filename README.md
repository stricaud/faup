<p align="center"><img src="doc/images/faup-logo.png"/></p>

**Faup** stands for Finally An Url Parser and is a library and command line tool to parse URLs and normalize fields with two constraints:
 1. Work with real-life urls (resilient to badly formated ones)
 2. Be fast: no allocation for string parsing and read characters only once

 * **Webpage**: [http://stricaud.github.io/faup/](http://stricaud.github.io/faup/)

 * **Source**: [https://github.com/stricaud/faup][github]
 * **Issues**: [https://github.com/stricaud/faup/issues][issues]
 * **Mailing List**: [libfaup@googlegroups.com](https://groups.google.com/d/forum/libfaup)

## Documentation

* [Library API documentation][libdoc]
* [Command Line Tool][clidoc]

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


Extract only the TLD field
--------------------------

	$ faup -f tld slashdot.org
	org

	$ faup -f tld www.bbc.co.uk
	co.uk

	$ faup -f tld -t www.bbc.co.uk
	uk


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
