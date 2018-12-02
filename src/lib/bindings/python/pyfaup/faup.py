import sys
import chardet

from .functions import *

class UrlNotDecoded(Exception):
    pass

class Faup(object):
    """
    Faup Python Library 
    """
    def __init__(self):
        self.options = faup_options_new()
        self.fh = faup_init(self.options)
        self.decoded = False
        self.retval = {}
        self.python_ver = sys.version_info.major

    def __del__(self):
        faup_terminate(self.fh)
        faup_options_free(self.options)

    def decode_str(self, string):        
        if self.python_ver >= 3:
            if string:
                return string.decode("utf-8")
        
        return string
        
    def decode(self, url):
        """
        This function creates a dict of all the url fields.

        :param url: The URL to normalize
        """
        self._url = None

        if self.python_ver >= 3:
            self._url = bytes(url.encode('utf-8'))
        else:
            self._url = bytes(url)
        
        self._url = faup_decode(self.fh, self._url, len(self._url))
        self.decoded = True
        self.retval = {}
    
    @property    
    def url(self):
        return self._url
        
    def get_version(self):
        return faup_get_version()

    def _get_param_from_pos_and_size(self, pos, size):
        if pos < 0:
            return None

        else:
            return self._url[pos:(pos+size)]

    def get_scheme(self):
        """
        Get the scheme of the url given in the decode function

        :returns: The URL scheme
        """
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_scheme_pos(self.fh)
        size = faup_get_scheme_size(self.fh)

        return self.decode_str(self._get_param_from_pos_and_size(pos, size))

    def get_credential(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_credential_pos(self.fh)
        size = faup_get_credential_size(self.fh)

        return self.decode_str(self._get_param_from_pos_and_size(pos, size))

    def get_subdomain(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")


        pos = faup_get_subdomain_pos(self.fh)
        size = faup_get_subdomain_size(self.fh)
        return self.decode_str(self._get_param_from_pos_and_size(pos, size))

    def get_domain(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_domain_pos(self.fh)
        size = faup_get_domain_size(self.fh)

        return self.decode_str(self._get_param_from_pos_and_size(pos, size))

    def get_domain_without_tld(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_domain_without_tld_pos(self.fh)
        size = faup_get_domain_without_tld_size(self.fh)

        return self.decode_str(self._get_param_from_pos_and_size(pos, size))
        
    def get_host(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_host_pos(self.fh)
        size = faup_get_host_size(self.fh)

        return self.decode_str(self._get_param_from_pos_and_size(pos, size))
        
    def get_tld(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_tld_pos(self.fh)
        size = faup_get_tld_size(self.fh)

        return self.decode_str(self._get_param_from_pos_and_size(pos, size))
            
    def get_port(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_port_pos(self.fh)
        size = faup_get_port_size(self.fh)

        return self.decode_str(self._get_param_from_pos_and_size(pos, size))

    def get_resource_path(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_resource_path_pos(self.fh)
        size = faup_get_resource_path_size(self.fh)

        return self.decode_str(self._get_param_from_pos_and_size(pos, size))

    def get_query_string(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_query_string_pos(self.fh)
        size = faup_get_query_string_size(self.fh)

        return self.decode_str(self._get_param_from_pos_and_size(pos, size))

    def get_fragment(self):
        if not self.decoded:
            raise UrlNotDecoded("You must call faup.decode() first")

        pos = faup_get_fragment_pos(self.fh)
        size = faup_get_fragment_size(self.fh)

        return self.decode_str(self._get_param_from_pos_and_size(pos, size))

    def get(self):        
        self.retval["scheme"] = self.get_scheme()
        self.retval["tld"] = self.get_tld()    
        self.retval["domain"] = self.get_domain()
        self.retval["domain_without_tld"] = self.get_domain_without_tld()
        self.retval["subdomain"] = self.get_subdomain()    
        self.retval["host"] = self.get_host()
        self.retval["port"] = self.get_port()
        self.retval["resource_path"] = self.get_resource_path()
        self.retval["query_string"] = self.get_query_string()
        self.retval["fragment"] = self.get_fragment()
        self.retval["url"] = self.url
        return self.retval
