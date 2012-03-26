Fast URL Library
================

The Fast URL Library provides:

* A library you can embed in your software (furll)
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

