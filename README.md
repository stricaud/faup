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
* [Working with Snapshots][snapshots]
* [Library API documentation][libdoc]

* [Frequently Asked Questions][faq]


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

## LUA support
Faup can be compiled without LUA support. In that case, CMake will output the following line
```
-- Could NOT find Lua51 (missing:  LUA_INCLUDE_DIR) 
```

If you want to add LUA functionnality you need to install lua development headers prior to the previous building steps. 

For example, on Redhat systems:
```
# yum -y install lua lua-devel
```


    
CMake 2.8 for Redhat/CentOS 6.x
-------------------------------
The following error may appears if you have an outdated version of CMake (just like Redhat and CentOS systems):

```
CMake Error at CMakeLists.txt:1 (cmake_minimum_required):
  CMake 2.8 or higher is required.  You are running version 2.6.4

-- Configuring incomplete, errors occurred!
```

To manually install CMake 2.8 on Redhat/CentOS systems use the sources and follow those instructions:

```
# Install dependencies
yum install ncurses-devel gcc gcc-c++ make

# Get the sources
cd /usr/local/
wget http://www.cmake.org/files/v2.8/cmake-2.8.12.2.tar.gz
tar xzf cmake-2.8.12.2.tar.gz
cd cmake-2.8.12.2

# Compile and install the sources
./configure
make
make install

# clean the env
cd /usr/local
rm -rf cmake-2.8.12.2 cmake-2.8.12.2.tar.gz

# adding cmake to the PATH 
echo "PATH=/usr/local/bin/:\$PATH" > /etc/profile.d/cmake28.sh 
source /etc/profile
```
    
    
FAQ
---

### Why do I receive the error “libfaupl.so.1: cannot open shared object file” when trying to run faup?

If you get a shared library loading error similar to the following when trying to run faup, its probably due to your platform doesn't include the `/usr/local/lib` shared library directory by default  (ex: [Ubuntu](http://developer.ubuntu.com/packaging/html/libraries.html)/[Debian](http://www.debian.org/doc/debian-policy/ch-sharedlibs.html#s-ldconfig))  or the directory where faup has its shared library installed:

```
$ faup
faup: error while loading shared libraries: libfaupl.so.1: cannot open shared object file: No such file or director
```

A good way to see which shared libraries are loaded by faup is by using the `ldd` command:
```
$ ldd /usr/local/bin/faup 
	linux-vdso.so.1 =>  (0x00007fff89735000)
	libfaupl.so.1 => not found
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f55a6082000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f55a641a000)

```
To update the faup shared library path, you can use the `ldconfig` command. For example if faup libraries are installed in `/usr/local/lib`, you can add the path as follows:
```
$ echo '/usr/local/lib' | sudo tee -a /etc/ld.so.conf.d/faup.conf
$ ldconfig
$ ldd /usr/local/bin/faup 
	linux-vdso.so.1 =>  (0x00007fff550d5000)
	libfaupl.so.1 => /usr/local/lib/libfaupl.so.1 (0x00007f41f9102000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f41f8d78000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f41f9317000)

```

[github]: https://github.com/stricaud/faup
[issues]: https://github.com/stricaud/faup/issues
[libdoc]: doc/library.md
[clidoc]: doc/cli.md
[faq]: doc/faq.md
[snapshots]: doc/snapshots.md
