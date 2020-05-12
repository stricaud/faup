*********************************************
pyFaup - Python bindings for the Faup library
*********************************************

Faup is a URL parser, this is the Python library.

Examples
--------

Example 1: Parse a URL
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

		from pyfaup.faup import Faup

		f = Faup()
		f.decode("https://pypi.org/project/pyfaup")
		print("TLD:" + f.get_tld())
		

This will extract all the TLDs. Replace get_tld() with get() to grab all the parsed items. 

Example 2: Multithreading
~~~~~~~~~~~~~~~~~~~~~~~~~

Reading from a file one url per line in multiple threads (example contributed by Sebastien Larinier):

.. code-block:: python

		from pyfaup.faup import Faup
		import threading

		NUM_THREADS=5
		URLS_TO_READ="your_urls_file.txt"
		
		class ThreadFaup(threading.Thread):
		    def __init__(self,list_url,f):
		        self.list_url=list_url
			threading.Thread.__init__(self)
			self.f=f
		    def run(self):
		        for url in self.list_url:
			self.f.decode(url)
			print self.f.get()

		with open(URLS_TO_READ,'r') as fd:
		    for line in fd:
		        line=line.replace('\r\n','')
			list_url.append(line)

		f=Faup()
		for i in range(0,NUM_THREADS):
		    t=ThreadFaup(list_url,f)
		    t.start()

