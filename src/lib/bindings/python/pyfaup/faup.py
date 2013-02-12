from .functions import *
from .tldextract import TLDExtract
class UrlNotDecoded(Exception):
    pass

class Faup(object):    

    def __init__(self):
        self.fh = faup_init()
        self.decoded = False
        self.fast=True
        self.tldex=None
    def __del__(self):
        faup_terminate(self.fh)

    def decode(self, url,fast=True):
        self.url = url
        faup_decode(self.fh, self.url, len(url))
        self.decoded = True
        self.fast=fast

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
            if self.tldex==None:
                self.tldex=TLDExtract()
            self.tldex._extract(self.get_host())
            return getattr(self.tldex, 'subdomain')



    def get_domain(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")
        if self.fast:
            pos = faup_get_domain_pos(self.fh)
            size = faup_get_domain_size(self.fh)

            return self._get_param_from_pos_and_size(pos, size)
        else:
            if self.tldex==None:
                self.tldex=TLDExtract()
            self.tldex._extract(self.get_host())
            return getattr(self.tldex, 'domain')
            
        
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
            if self.tldex==None:
                self.tldex=TLDExtract()
            self.tldex._extract(self.get_host())
            return getattr(self.tldex, 'tld')
            
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
        retval = {}
        retval["scheme"] = self.get_scheme()
        retval["credential"] = self.get_credential()
        retval["subdomain"] = self.get_subdomain()
        retval["domain"] = self.get_domain()
        retval["host"] = self.get_host()
        retval["tld"] = self.get_tld()
        retval["port"] = self.get_port()
        retval["resource_path"] = self.get_resource_path()
        retval["query_string"] = self.get_query_string()
        retval["fragment"] = self.get_fragment()
        return retval
