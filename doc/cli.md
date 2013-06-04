## Faup CLI

faup comes out of the box with a command line tool that can be used to work with urls with arguments or pipes. Things are quite straightforward and the whole idea of faup was to ease your job of url parsing, not having a new 'tc' experience :)

Output
------

An URL can be output in two formats: CSV and JSON. The default is CSV. For example, the output can be changed using the '-o' argument providing 'json' or 'csv'.

	$ echo "http://www.github.com" |faup
	http,,www,github.com,www.github.com,com,,,,

	$ faup http://www.github.com
	http,,www,github.com,www.github.com,com,,,,

	$ faup -o json http://www.github.com
	{
		"scheme": "http",
		"credential": "",
		"subdomain": "www",
		"domain": "github.com",
		"host": "www.github.com",
		"tld": "com",
		"port": "",
		"resource_path": "",
		"query_string": "",
		"fragment": "",
	}

Output only one field
---------------------

The '-f' argument allow you to select the field that you want to output. You can choose between scheme, credential, subdomain, domain, host, tld, port, resource_path, query_string or fragment. To display
only TLDs, simply do:

	$ faup -f tld http://www.github.com
	com

TLDs
----

TLD can be extracted in two different manners. Originaly, we were lazy and in .co.uk, only .uk was extracted. However, the question of extracting TLD > 1 started to beat the game. Thank to the amazing work
of Sebastien Larinier and Cedric Leroux, we had in the end a native implementation of something that became the default in the end since it did not introduced too much overhead, by using the algorithm of the year
awarded in 1973!

Still, the behavior of extracting only TLD of level 1 only can be asked by using the '-t' flag.

	$ faup -f tld http://www.google.co.uk
	co.uk

	$ faup -f tld -t http://www.github.com
	uk

To have co.uk extracted, we match against a list, this is list maintained by Mozilla dudes. So a *big* thank you Mozilla!

We provide a default list when faup is installed in $INSTALL_DIR/share/faup/mozilla.tlds. That list can be updated using the '-u' argument. 

	$ faup -u

The list is downloaded in $INSTALL_DIR/share/faup/ if we can write there, otherwise, it will be in your $HOME/.faup/ 

Printing line numbers
---------------------

Because it can be convenient to get the line number where the url was normalized, we have a '-l' argument. Of course, if the URL does not come from a pipe, only 0 will be displayed.

	$ echo "http://www.slashdot.org:8080" > test-url.txt
	$ echo "http://www.github.com/stricaud/faup" >> test-url.txt
	$ cat test-url.txt |faup -l
	1,http,,www,slashdot.org,www.slashdot.org,org,8080,,,
	2,http,,www,github.com,www.github.com,com,,/stricaud/faup,,

CSV Specific options
--------------------

Some options are just for the CSV output, such as printing headers ('-p') or change the delimiter character ('-d').

	$ cat test-url.txt |faup -p -d;
	scheme;credential;subdomain;domain;host;tld;port;resource_path;query_string;fragment
	http;;www;slashdot.org;www.slashdot.org;org;8080;;;
	http;;www;github.com;www.github.com;com;;/stricaud/faup;;

