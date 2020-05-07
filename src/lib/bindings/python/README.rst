*********************************************
pyFaup - Python bindings for the Faup library
*********************************************

Faup is a URL parser, this is the Python library.

Examples
--------

Parse a URL

.. code-block:: python

		from pyfaup.faup import Faup

		f = Faup()
		f.decode("https://pypi.org/project/pyfaup")
		print("TLD:" + f.get_tld())
		

This will extract all the TLDs
