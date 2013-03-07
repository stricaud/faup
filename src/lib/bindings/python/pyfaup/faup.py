from .functions import *
from .tldextract import TLDExtract
class UrlNotDecoded(Exception):
    pass

class Faup(object):    

    def __init__(self):
        self.fh = faup_init()
        self.decoded = False
        self.fast=False
        self.tld=TLDExtract()
        self.retval = {}
        self.get_domain_function='get_domain'
        self.get_subdomain_function='get_subdomain'
        self.get_tld_function='get_tld'
        
        if self.fast:
            self.get_domain_function=self.get_domain_function+'_fast'
            self.get_subdomain_function=self.get_subdomain_function+'_fast'
            self.get_tld_function=self.get_tld_function+'_fast'
        else:
            self.get_domain_function=self.get_domain_function+'_slow'
            self.get_subdomain_function=self.get_subdomain_function+'_slow'
            self.get_tld_function=self.get_tld_function+'_slow'
            
    def __del__(self):
        faup_terminate(self.fh)
    def decode(self, url,fast=True):
        self.url = url
        faup_decode(self.fh, self.url, len(url))
        self.decoded = True
        self.fast=fast
        self.retval = {}
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
        return getattr(self, self.get_subdomain_function)()
        
    def get_subdomain_fast(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")
        pos = faup_get_subdomain_pos(self.fh)
        size = faup_get_subdomain_size(self.fh)
        return self._get_param_from_pos_and_size(pos, size)
    
    def get_subdomain_slow(self):
        subdomain_attr=getattr(self.tld, 'subdomain')
        if subdomain_attr is None:    
            self.tld._extract(self.get_host())
        return subdomain_attr

    def get_domain(self):
        return getattr(self, self.get_domain_function)()
    
    def get_domain_fast(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")
        pos = faup_get_domain_pos(self.fh)
        size = faup_get_domain_size(self.fh)
        return self._get_param_from_pos_and_size(pos, size)
    
    def get_domain_slow(self):
        domain_attr = getattr(self.tld, 'domain')
        
        if domain_attr is None:
                self.tld._extract(self.get_host())
        return domain_attr
                
    def get_host(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_host_pos(self.fh)
        size = faup_get_host_size(self.fh)

        return self._get_param_from_pos_and_size(pos, size)
    
    def get_tld(self):
        return getattr(self,self.get_tld_function)()
    def get_tld_fast(self):
        if not self.decoded:
                raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_tld_pos(self.fh)
        size = faup_get_tld_size(self.fh)
        return self._get_param_from_pos_and_size(pos, size)
    
    def get_tld_slow(self):
        tld_attr=getattr(self.tld, 'tld')
        if tld_attr is None:
            self.tld._extract(self.get_host())
        return tld_attr
            
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
