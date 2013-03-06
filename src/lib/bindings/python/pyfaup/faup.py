import sys

from .functions import *
from .tldextract import TLDExtract

class UrlNotDecoded(Exception):
    pass

class Faup(object):
    def __init__(self):
        self.fh = faup_init()
        self.decoded = False
        self.fast=True
        self.tld=TLDExtract()
        self.retval = {}

    def __del__(self):
        faup_terminate(self.fh)

    def decode(self, url,fast=True):
        self.url = None

        # Alright, I assume no one is using python 1.x nor 4.x
        if sys.version.split('.')[0].split('.')[0]=='3':
            self.url = bytes(url,'utf-8')
        if sys.version.split('.')[0].split('.')[0]=='2':
            self.url = bytes(url)

        faup_decode(self.fh, self.url, len(url))
        self.decoded = True
        self.fast=fast
        self.retval = {}
        if self.fast==False:
            if self.tld !=None:
                self.tld.init()                
        
    def get_version(self):
        return faup_get_version()

    def _get_param_from_pos_and_size(self, pos, size):
        if pos < 0:
            return None

        else:
            return self.url[pos:(pos+size)]

    def get_scheme(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_scheme_pos(self.fh)
        size = faup_get_scheme_size(self.fh)

        return self._get_param_from_pos_and_size(pos, size)

    def get_credential(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_credential_pos(self.fh)
        size = faup_get_credential_size(self.fh)

        return self._get_param_from_pos_and_size(pos, size)

    def get_subdomain(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")
        if self.fast:
            pos = faup_get_subdomain_pos(self.fh)
            size = faup_get_subdomain_size(self.fh)
            return self._get_param_from_pos_and_size(pos, size)
        else:
            if self.tld==None:
                self.tld=TLDExtract()
            if getattr(self.tld, 'subdomain')==None:    
                self.tld._extract(self.get_host())
            subdomain=getattr(self.tld, 'subdomain')
            return subdomain



    def get_domain(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")
        if self.fast:
            pos = faup_get_domain_pos(self.fh)
            size = faup_get_domain_size(self.fh)

            return self._get_param_from_pos_and_size(pos, size)
        else:
            if self.tld==None:
                self.tld=TLDExtract()
            if getattr(self.tld, 'domain')==None:
                self.tld._extract(self.get_host())
            domaine=getattr(self.tld, 'domain')    
            return domaine
            
        
    def get_host(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_host_pos(self.fh)
        size = faup_get_host_size(self.fh)

        return self._get_param_from_pos_and_size(pos, size)
        
    def get_tld(self):
        if self.fast:
            if not self.decoded:
                raise UrlNotDecoded("You must call faup.decode() first")

            pos = faup_get_tld_pos(self.fh)
            size = faup_get_tld_size(self.fh)

            return self._get_param_from_pos_and_size(pos, size)
        else:
            if self.tld==None:
                self.tld=TLDExtract()
            if getattr(self.tld, 'tld')==None:
                self.tld._extract(self.get_host())
            tld=getattr(self.tld, 'tld')
            return tld
            
    def get_port(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_port_pos(self.fh)
        size = faup_get_port_size(self.fh)

        return self._get_param_from_pos_and_size(pos, size)

    def get_resource_path(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_resource_path_pos(self.fh)
        size = faup_get_resource_path_size(self.fh)

        return self._get_param_from_pos_and_size(pos, size)

    def get_query_string(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_query_string_pos(self.fh)
        size = faup_get_query_string_size(self.fh)

        return self._get_param_from_pos_and_size(pos, size)

    def get_fragment(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_fragment_pos(self.fh)
        size = faup_get_fragment_size(self.fh)

        return self._get_param_from_pos_and_size(pos, size)

    def get(self):        
        self.retval["scheme"] = self.get_scheme()
        self.retval["tld"] = self.get_tld()    
        self.retval["domain"] = self.get_domain()
        self.retval["subdomain"] = self.get_subdomain()    
        self.retval["host"] = self.get_host()
        self.retval["port"] = self.get_port()
        self.retval["resource_path"] = self.get_resource_path()
        self.retval["query_string"] = self.get_query_string()
        self.retval["fragment"] = self.get_fragment()
        return self.retval
