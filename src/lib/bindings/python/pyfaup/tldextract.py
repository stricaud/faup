# -*- coding: utf-8 -*-
"""
this code is inspired: https://github.com/john-kurkowski/tldextract
`tldextract` accurately separates the gTLD or ccTLD (generic or country code
top-level domain) from the registered domain and subdomains of a URL.

    >>> import tldextract
    >>> tldextract.extract('http://forums.news.cnn.com/')
    ExtractResult(subdomain='forums.news', domain='cnn', tld='com')
    >>> tldextract.extract('http://forums.bbc.co.uk/') # United Kingdom
    ExtractResult(subdomain='forums', domain='bbc', tld='co.uk')
    >>> tldextract.extract('http://www.worldbank.org.kg/') # Kyrgyzstan
    ExtractResult(subdomain='www', domain='worldbank', tld='org.kg')

`ExtractResult` is a namedtuple, so it's simple to access the parts you want.

    >>> ext = tldextract.extract('http://forums.bbc.co.uk')
    >>> ext.domain
    'bbc'
    >>> '.'.join(ext[:2]) # rejoin subdomain and domain
    'forums.bbc'
"""


from functools import wraps
from operator import itemgetter
import errno
import logging
import os
import re
import socket
import sys
import binascii

try:
    import pickle as pickle
except ImportError:
    import pickle

try:
    import pkg_resources
except ImportError:
    class pkg_resources(object):
        """Fake pkg_resources interface which falls back to getting resources
        inside `tldextract`'s directory.
        """
        @classmethod
        def resource_stream(cls, package, resource_name):
            moddir = os.path.dirname(__file__)
            f = os.path.join(moddir, resource_name)
            return open(f)

LOG = logging.getLogger("tldextract")





class TLDExtract(object):
    _instance=None
    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super(TLDExtract, cls).__new__(
                                cls, *args, **kwargs)
        return cls._instance
    
    def __init__(self, fetch=True, cache_file=''):
        """
        Constructs a callable for extracting subdomain, domain, and TLD
        components from a URL.

        If fetch is True (the default) and no cached TLD set is found, this
        extractor will fetch TLD sources live over HTTP on first use. Set to
        False to not make HTTP requests. Either way, if the TLD set can't be
        read, the module will fall back to the included TLD set snapshot.

        Specifying cache_file will override the location of the TLD set.
        Defaults to /path/to/tldextract/.tld_set.

        """
        self.fetch = fetch
        self.cache_file = cache_file or os.path.join(os.path.dirname(__file__), '.tld_set')
        self._extractor = None
        self.tld=''
        self.domain=''
        self.subdomain=''
    def __call__(self, host):
        """
        Takes a string URL and splits it into its subdomain, domain, and
        gTLD/ccTLD component.

        >>> extract = TLDExtract()
        >>> extract('http://forums.news.cnn.com/')
        ExtractResult(subdomain='forums.news', domain='cnn', tld='com')
        >>> extract('http://forums.bbc.co.uk/')
        ExtractResult(subdomain='forums', domain='bbc', tld='co.uk')
        """
        return self._extract(host)

    def _extract(self, host):                
        registered_domain, tld = self._get_tld_extractor().extract(host)    
        subdomain, _, domain = registered_domain.rpartition(b'.')
        self.tld=tld
        self.subdomain=subdomain
        self.domain=domain



    def _get_tld_extractor(self):
        if self._extractor:
            return self._extractor

        cached_file = self.cache_file
        try:
            with open(cached_file) as f:
                self._extractor = _PublicSuffixListTLDExtractor(pickle.load(f))
                return self._extractor
        except IOError as ioe:
            file_not_found = ioe.errno == errno.ENOENT
            if not file_not_found:
              LOG.error("error reading TLD cache file %s: %s", cached_file, ioe)
        except Exception as ex:
            LOG.error("error reading TLD cache file %s: %s", cached_file, ex)

        tlds = frozenset()
        if self.fetch:
            tld_sources = (_PublicSuffixListSource,)     
            tlds = frozenset(binascii.a2b_qp(tld) for tld_source in tld_sources for tld in tld_source())            
        if not tlds:
            with pkg_resources.resource_stream(__name__, '.tld_set_snapshot') as snapshot_file:
                self._extractor = _PublicSuffixListTLDExtractor(pickle.load(snapshot_file))
                return self._extractor

        LOG.info(b"computed TLDs: [%s, ...]", b', '.join(list(tlds)[:10]))
        if LOG.isEnabledFor(logging.DEBUG):
            import difflib
            with pkg_resources.resource_stream(__name__, '.tld_set_snapshot') as snapshot_file:
                snapshot = sorted(pickle.load(snapshot_file))
            new = sorted(tlds)
            for line in difflib.unified_diff(snapshot, new, fromfile=".tld_set_snapshot", tofile=cached_file):
                print(line)

        try:
            with open(cached_file, 'wb') as f:
                pickle.dump(tlds, f)

        except IOError as e:
            LOG.warn("unable to cache TLDs in file %s: %s", cached_file, e)
        self._extractor = _PublicSuffixListTLDExtractor(tlds)
        return self._extractor

def _fetch_page(url):
        if sys.version.split('.')[0].split('.')[0]=='3':
            try:
                import urllib.request, urllib.error, urllib.parse
                import urllib.parse
                return str(urllib.request.urlopen(url).read(), 'utf-8')
            except urllib.error.URLError as e:
                LOG.error(e)
        if sys.version.split('.')[0].split('.')[0]=='2':
            import urllib2
            try:
                return str(urllib2.urlopen(url).read())
            except urllib2.URLError as e:
                LOG.error(e)
        
        return ''

def _PublicSuffixListSource():
    page = _fetch_page('http://mxr.mozilla.org/mozilla-central/source/netwerk/dns/effective_tld_names.dat?raw=1')

    tld_finder = re.compile(r'^(?P<tld>[.*!]*\w[\S]*)', re.UNICODE | re.MULTILINE)
    #tlds = [m.group('tld') for m in tld_finder.finditer(page)]
    for m in tld_finder.finditer(page):
        yield m.group('tld')

class _PublicSuffixListTLDExtractor(object):
    def __init__(self, tlds):
        self.tlds = tlds

    def extract(self, netloc):
        spl = netloc.split(b'.')
        for i in range(len(spl)):
            maybe_tld = b'.'.join(spl[i:])
            exception_tld = b'!' + maybe_tld
            if exception_tld in self.tlds:
                return b'.'.join(spl[:i+1]), '.'.join(spl[i+1:])

            wildcard_tld = b'*.' + b'.'.join(spl[i+1:])
            if wildcard_tld in self.tlds or maybe_tld in map(bytes, self.tlds):
                return b'.'.join(spl[:i]), maybe_tld

        return netloc, b''
